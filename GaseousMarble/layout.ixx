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
        failed_to_line_break = -2,
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
            case LayoutError::failed_to_line_break:
                return "Failed to find line break points.";
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

    export usize utf8_size(u32 ch) noexcept {
        return U8_LENGTH(ch);
    }

    export bool is_white_space(u32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

    export template <class Fn>
    bool unicode_for_each(std::string_view text, Fn&& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, text.data(), text.size(), &error) };
        if (!iter) {
            return false;
        }

        while (true) {
            auto ch{ static_cast<u32>(UTEXT_NEXT32(iter.get())) };
            if (ch == -1 || !func(ch)) {
                return true;
            }
        }
    }

    export struct LineBreakToken {
        usize first;
        usize last;
        bool hard;
    };

    export template <class Fn>
    bool line_break_for_each(std::string_view text, Fn&& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, text.data(), text.size(), &error) };
        if (!iter) {
            return false;
        }

        Handle<UBreakIterator*, ubrk_close> breaker{ ubrk_open(UBRK_LINE, "", nullptr, 0, &error) };
        if (!breaker) {
            return false;
        }

        ubrk_setUText(breaker.get(), iter.get(), &error);
        if (error > 0) {
            return false;
        }

        usize first{};
        while (true) {
            auto last{ static_cast<usize>(ubrk_next(breaker.get())) };
            if (last == -1 || !func({ first, last, ubrk_getRuleStatus(breaker.get()) == UBRK_LINE_HARD })) {
                return true;
            }

            first = last;
        }
    }

    export struct LayoutOption {
        std::pair<const Font*, usize> font{};
        bool justified{};
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
        usize first;
        usize last;
        usize visual_last;
        f32 advance;
        f32 width;

        static LayoutToken from(const LineBreakToken& lb_token, std::string_view text, const LayoutOption& option) {
            LayoutToken token{ lb_token.first, lb_token.last, lb_token.first };
            std::string_view token_text{ text.data() + lb_token.first, lb_token.last - lb_token.first };
            auto& glyphs{ option.font.first->glyphs() };
            usize next{ token.first };
            if (!unicode_for_each(token_text, [&](u32 ch) noexcept {
                    next += utf8_size(ch);

                    auto glyph_iter{ glyphs.find(ch) };
                    if (glyph_iter == glyphs.end()) {
                        return true;
                    }

                    auto& [sprite_x, sprite_y, width, advance, left]{ glyph_iter->second };
                    if (!is_white_space(ch)) {
                        token.visual_last = next;
                        token.width = token.advance + left + width;
                    }

                    token.advance += advance + option.letter_spacing;
                    if (is_white_space(ch)) {
                        token.advance += option.word_spacing;
                    }

                    return true;
                })) {
                throw std::system_error{ LayoutError::failed_to_decode };
            }

            return token;
        }
    };

    export struct LayoutLine {
        std::vector<LayoutToken> tokens;
        bool hard;
        f32 width;
        f32 height;
        f32 justified_spacing;
    };

    export struct Layout {
        std::string text;
        std::vector<LayoutLine> lines;
        f32 width;
        f32 height;

        static Layout from(std::string_view text, const LayoutOption& option) {
            if (!option.is_valid()) {
                throw std::system_error{ LayoutError::invalid_option };
            }

            auto glyph_height{ static_cast<f32>(option.font.first->glyph_height()) };
            Layout layout{ std::string{ text } };
            LayoutLine line{};
            std::optional<LayoutToken> pending;
            f32 cursor{};

            auto push_token{ [&] noexcept {
                f32 next{ cursor + pending->advance };
                if (pending->first != pending->visual_last) {
                    line.width = cursor + pending->width;
                    line.tokens.emplace_back(std::move(*pending));
                }

                pending.reset();
                cursor = next;
            } };

            auto push_line{ [&](bool last = false) noexcept {
                if (option.justified && option.max_line_length != 0 && !line.hard && line.tokens.size() > 1) {
                    line.justified_spacing = (option.max_line_length - line.width) / (line.tokens.size() - 1);
                    line.width = option.max_line_length;
                }

                line.height = glyph_height;
                if (!last) {
                    if (line.hard) {
                        line.height += option.paragraph_spacing;
                    }
                    line.height *= option.line_height;
                }

                layout.width = std::max(layout.width, line.width);
                layout.height += line.height;
                layout.lines.emplace_back(std::move(line));

                line = {};
                cursor = 0;
            } };

            if (!line_break_for_each(layout.text, [&](const LineBreakToken& lb_token) {
                    auto token{ LayoutToken::from(lb_token, layout.text, option) };
                    if (pending) {
                        bool overflow{ option.max_line_length != 0
                                       && cursor + pending->advance + token.width > option.max_line_length
                                       && token.first != token.visual_last };
                        push_token();
                        if (overflow) {
                            push_line();
                        }
                    }

                    pending = std::move(token);
                    if (lb_token.hard) {
                        line.hard = true;
                        push_token();
                        push_line();
                    }

                    return true;
                })) {
                throw std::system_error{ LayoutError::failed_to_line_break };
            }

            if (pending) {
                push_token();
            }

            line.hard = true;
            push_line(true);
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
                    value.justified,
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
