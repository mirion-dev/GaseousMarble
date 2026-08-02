module;

#include <cassert>
#include <icu.h>

export module gm.layout;

import std;
import gm.types;
import gm.utils;
import gm.font;

namespace gm {

    export enum class LayoutError {
        failed_to_decode     = -1,
        failed_to_word_break = -2,
        invalid_option       = -3
    };

}

export template <>
struct std::is_error_code_enum<gm::LayoutError> : std::true_type {};

namespace gm {

    class LayoutErrorCategory : public std::error_category {
    public:
        const char* name() const noexcept {
            return "gm.layout";
        }

        std::string message(int value) const {
            switch (static_cast<LayoutError>(value)) {
            case LayoutError::failed_to_decode:
                return "Failed to decode text.";
            case LayoutError::failed_to_word_break:
                return "Failed to break text into words.";
            case LayoutError::invalid_option:
                return "Invalid layout option.";
            }

            return "Unknown layout error.";
        }
    };

    export const std::error_category& layout_error_category() noexcept {
        static LayoutErrorCategory category;
        return category;
    }

    export std::error_code make_error_code(LayoutError error) noexcept {
        return { static_cast<int>(error), layout_error_category() };
    }

    export bool is_white_space(u32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

    export bool is_line_break(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_LINE_BREAK)) {
        case U_LB_MANDATORY_BREAK:
        case U_LB_CARRIAGE_RETURN:
        case U_LB_LINE_FEED:
        case U_LB_NEXT_LINE:
            return true;
        default:
            return false;
        }
    }

    export bool is_wide(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH)) {
        case U_EA_FULLWIDTH:
        case U_EA_WIDE:
            return true;
        default:
            return false;
        }
    }

    export template <class Fn>
    bool unicode_for_each(std::string_view text, Fn&& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, text.data(), text.size(), &error) };
        if (!iter) {
            return false;
        }

        while (true) {
            auto ch{ static_cast<u32>(utext_next32(iter.get())) };
            if (ch == -1 || !func(ch)) {
                return true;
            }
        }
    }

    export template <class Fn>
    bool word_break_for_each(std::string_view text, Fn&& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, text.data(), text.size(), &error) };
        if (!iter) {
            return false;
        }

        Handle<UBreakIterator*, ubrk_close> breaker{ ubrk_open(UBRK_WORD, "", nullptr, 0, &error) };
        if (!breaker) {
            return false;
        }

        ubrk_setUText(breaker.get(), iter.get(), &error);
        if (error > 0) {
            return false;
        }

        const char* ptr{ text.data() };
        usize first{};
        while (true) {
            auto last{ static_cast<usize>(ubrk_next(breaker.get())) };
            if (last == -1
                || !func(
                    std::string_view{ ptr + first, ptr + last }, static_cast<u32>(ubrk_getRuleStatus(breaker.get()))
                )) {
                return true;
            }
            first = last;
        }
    }

    export struct LayoutOption {
        std::pair<const Font*, usize> font{};
        f32 letter_spacing{};
        f32 word_spacing{};
        f32 paragraph_spacing{};
        f32 line_height{ 1 };
        f32 max_line_length{};

        bool is_valid() const noexcept {
            return font.first != nullptr && font.second > 0 && max_line_length >= 0;
        }

        friend bool operator==(const LayoutOption& left, const LayoutOption& right) noexcept = default;
    };

    export struct LayoutToken {
        std::string str;
        bool continuous;
    };

    export struct LayoutLine {
        std::vector<LayoutToken> tokens;
        f32 width;
        f32 height;
        f32 justified_spacing;
    };

    export struct Layout {
        std::vector<LayoutLine> lines;
        f32 width{};
        f32 height{};

        static Layout from(std::string_view text, const LayoutOption& option) {
            if (!option.is_valid()) {
                throw std::system_error{ LayoutError::invalid_option };
            }

            auto glyph_height{ static_cast<f32>(option.font.first->glyph_height()) };
            auto& glyphs{ option.font.first->glyphs() };

            const char* first{ text.data() };
            const char* last{ first };
            bool cont{};

            Layout layout;
            LayoutLine line{ .height = glyph_height };
            f32 cursor{};
            usize justified_count{};

            auto push_token{ [&] noexcept {
                if (first == last) {
                    return;
                }

                if (!cont) {
                    ++justified_count;
                }
                line.tokens.emplace_back(std::string{ first, last }, cont);
            } };

            auto push_line{ [&](bool hard = false, bool last = false) noexcept {
                push_token();

                if (!hard && justified_count > 1) {
                    line.justified_spacing = (option.max_line_length - line.width) / (justified_count - 1);
                    line.width = option.max_line_length;
                }

                if (!last) {
                    line.height *= option.line_height;
                }

                layout.width = std::max(layout.width, line.width);
                layout.height += line.height;
                layout.lines.emplace_back(std::move(line));

                line = { .height = glyph_height };
                cursor = 0;
                justified_count = 0;
            } };

            auto push_word{ [&](std::string_view word, u32 type) {
                const char* word_begin{ word.data() };
                const char* word_end{ word_begin + word.size() };
                f32 next_cursor{ cursor }, next_line_width{ line.width };
                bool first_ch{ true };
                bool line_break{};
                if (!unicode_for_each(word, [&](u32 ch) noexcept {
                        if (first_ch) {
                            first_ch = false;

                            if (is_line_break(ch)) {
                                line.height += option.paragraph_spacing;
                                push_line(true);
                                first = word_end;
                                cont = false;
                                line_break = true;
                                return false;
                            }

                            bool word_cont{ type >= UBRK_WORD_KANA || type == UBRK_WORD_NONE && is_wide(ch) };
                            if (cont != word_cont) {
                                push_token();
                                first = word_begin;
                                cont = word_cont;
                            }
                        }

                        auto glyph_iter{ glyphs.find(ch) };
                        if (glyph_iter == glyphs.end()) {
                            return true;
                        }

                        auto& [sprite_x, sprite_y, width, advance, left]{ glyph_iter->second };
                        if (option.max_line_length != 0
                            && cursor != 0
                            && next_cursor + left + width > option.max_line_length) {
                            next_cursor -= cursor;
                            push_line();
                            first = word_begin;
                        }

                        next_line_width = next_cursor + left + width;
                        next_cursor += advance + option.letter_spacing;
                        if (is_white_space(ch)) {
                            next_cursor += option.word_spacing;
                        }
                        if (cont) {
                            ++justified_count;
                        }
                        return true;
                    })) {
                    throw std::system_error{ LayoutError::failed_to_decode };
                }

                if (!line_break) {
                    cursor = next_cursor;
                    line.width = next_line_width;
                }

                last = word_end;
                return true;
            } };

            if (!word_break_for_each(text, push_word)) {
                throw std::system_error{ LayoutError::failed_to_word_break };
            }

            push_line(true, true);
            return layout;
        }
    };

    export struct LayoutSpecRef {
        std::string_view text;
        const LayoutOption& option;

        friend bool operator==(const LayoutSpecRef& left, const LayoutSpecRef& right) noexcept {
            return left.text == right.text && left.option == right.option;
        }
    };

    export struct LayoutSpec {
        std::string text;
        LayoutOption option;

        template <std::convertible_to<LayoutSpecRef> R>
        static LayoutSpec from(R&& spec) noexcept {
            auto [text, option]{ static_cast<LayoutSpecRef>(spec) };
            return { std::string{ text }, option };
        }

        operator LayoutSpecRef() const noexcept {
            return { text, option };
        }
    };

    export class LayoutCache {
        struct Hash : gm::Hash {
            using gm::Hash::operator();

            usize operator()(const LayoutOption& value) const noexcept {
                return hash_combine(
                    Hash{},
                    value.font.second,
                    value.letter_spacing,
                    value.word_spacing,
                    value.paragraph_spacing,
                    value.line_height,
                    value.max_line_length
                );
            }

            usize operator()(const LayoutSpecRef& value) const noexcept {
                return hash_combine(Hash{}, value.text, value.option);
            }
        };

        usize _cache_size{};

        std::list<std::pair<LayoutSpec, Layout>> _data;
        std::unordered_map<LayoutSpecRef, decltype(_data)::iterator, Hash> _map;

    public:
        LayoutCache() noexcept = default;

        explicit LayoutCache(usize cache_size) noexcept
            : _cache_size{ cache_size } {

            assert(_cache_size > 0);
        }

        LayoutCache(LayoutCache&&) noexcept = default;

        LayoutCache& operator=(LayoutCache&&) noexcept = default;

        operator bool() const noexcept {
            return _cache_size > 0;
        }

        usize cache_size() const noexcept {
            assert(*this);
            return _cache_size;
        }

        const Layout& get(const LayoutSpecRef& spec) {
            assert(*this && spec.option.is_valid());

            auto map_iter{ _map.find(spec) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return iter->second;
            }

            auto iter{ _data.emplace(_data.end(), LayoutSpec::from(spec), Layout::from(spec.text, spec.option)) };
            _map.try_emplace(iter->first, iter);

            if (_data.size() > _cache_size) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            return iter->second;
        }
    };

}
