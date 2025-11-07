module;

#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;
import :font;

namespace gm {

    export struct Text {
        struct Token {
            std::u16string_view str;
            bool continuous;
        };

        struct Line {
            std::vector<Token> tokens;
            f64 width;
            f64 height;
            f64 justified_spacing;
        };

        struct Layout {
            std::vector<Line> lines;
            f64 width;
            f64 height;
        };

        std::u16string str;
        Layout layout;
    };

    template <usize N>
        requires (N > 0)
    class Cache {
        std::list<std::pair<std::u8string, Text>> _list;
        std::unordered_map<std::u8string_view, typename decltype(_list)::iterator> _map;

    public:
        Cache() noexcept = default;

        Text* at(std::u8string_view str) noexcept {
            auto map_iter{ _map.find(str) };
            if (map_iter == _map.end()) {
                return {};
            }

            auto list_iter{ map_iter->second };
            _list.splice(_list.end(), _list, list_iter);
            return &list_iter->second;
        }

        template <class... Args>
        Text* emplace(std::u8string_view str, Args&&... args) noexcept {
            auto map_iter{ _map.find(str) };
            if (map_iter != _map.end()) {
                return {};
            }

            if (_map.size() == N) {
                _map.erase(_list.front().first);
                _list.pop_front();
            }

            auto list_iter{ _list.emplace(_list.end(), str, Text{ std::forward<Args>(args)... }) };
            _map.emplace_hint(map_iter, list_iter->first, list_iter);
            return &list_iter->second;
        }

        void clear() noexcept {
            _map.clear();
            _list.clear();
        }
    };

    export class Draw {
    public:
        struct Option {
            const Font* font{};
            i8 halign{ -1 };
            i8 valign{ -1 };
            bool justified{};
            u32 color_top{ 0xffffff };
            u32 color_bottom{ 0xffffff };
            f64 alpha{ 1 };
            f64 letter_spacing{};
            f64 word_spacing{};
            f64 paragraph_spacing{};
            f64 line_height{ 1 };
            f64 max_line_length{};
            f64 offset_x{};
            f64 offset_y{};
            f64 scale_x{ 1 };
            f64 scale_y{ 1 };
            f64 rotation{};
        };

        enum class Warning {
            no_warning     = 0,
            missing_glyphs = 1
        };

        enum class Error {
            no_error           = 0,
            invalid_encoding   = -1,
            failed_to_tokenize = -2,
            font_unspecified   = -3
        };

    private:
        static constexpr usize CACHE_SIZE{ 1024 };

        Option _option;
        Cache<CACHE_SIZE> _cache;

    public:
        const Option& option() const noexcept {
            return _option;
        }

        void set_option(const Option& option) noexcept {
            if (_option.font != option.font
                || _option.justified != option.justified
                || _option.letter_spacing != option.letter_spacing
                || _option.word_spacing != option.word_spacing
                || _option.paragraph_spacing != option.paragraph_spacing
                || _option.line_height != option.line_height
                || _option.max_line_length != option.max_line_length) {
                _cache.clear();
            }
            _option = option;
        }

        Result<Text, Warning, Error> create_text(std::u8string_view str) noexcept {
            auto ptr_text{ _cache.at(str) };
            if (ptr_text != nullptr) {
                return Payload{ *ptr_text, Warning::no_warning };
            }

            Text text{};
            Warning warning{};

            if (_option.font == nullptr) {
                return std::unexpected{ Error::font_unspecified };
            }

            auto& glyphs{ _option.font->glyphs() };
            auto glyph_height{ static_cast<f64>(_option.font->height()) };
            bool justified{ _option.justified };
            f64 letter_spacing{ _option.letter_spacing };
            f64 word_spacing{ _option.word_spacing };
            f64 paragraph_spacing{ _option.paragraph_spacing };
            f64 line_height{ _option.line_height };
            f64 max_line_length{ std::max(_option.max_line_length, 0.) / _option.scale_x };

            bool ok{};
            text.str.resize_and_overwrite(
                str.size(),
                [&](c16* ptr, usize) noexcept {
                    usize size{};
                    ok = unicode_for_each(
                        str,
                        [&](c32 ch) noexcept {
                            if (glyphs.contains(ch) || is_line_break(ch)) {
                                U16_APPEND_UNSAFE(ptr, size, ch);
                            }
                            else {
                                warning = Warning::missing_glyphs;
                            }
                            return true;
                        }
                    );
                    return size;
                }
            );
            if (!ok) {
                return std::unexpected{ Error::invalid_encoding };
            }

            const c16* first{ text.str.data() };
            const c16* last{ first };
            bool cont{};

            Text::Line line{};
            f64 cursor{};
            usize justified_count{};

            auto push_token{
                [&] noexcept {
                    if (first == last) {
                        return;
                    }
                    if (!cont) {
                        ++justified_count;
                    }
                    line.tokens.emplace_back(std::u16string_view{ first, last }, cont);
                }
            };

            auto push_line{
                [&](bool auto_wrap = false, bool last = false) noexcept {
                    push_token();

                    line.height = glyph_height;
                    if (auto_wrap && justified && justified_count > 1) {
                        line.justified_spacing = (max_line_length - line.width) / (justified_count - 1);
                        line.width = max_line_length;
                    }

                    if (!last) {
                        line.height *= line_height;
                    }

                    text.layout.lines.emplace_back(std::move(line));
                    text.layout.width = std::max(text.layout.width, line.width);
                    text.layout.height += line.height;

                    line = {};
                    cursor = 0;
                    justified_count = 0;
                }
            };

            auto push_word{
                [&](std::u16string_view word, i32 type) noexcept {
                    const c16* word_begin{ word.data() };
                    const c16* word_end{ word_begin + word.size() };
                    f64 next_cursor{ cursor }, next_line_width;
                    bool first_ch{ true };
                    if (unicode_for_each(
                        word,
                        [&](c32 ch) noexcept {
                            if (first_ch) {
                                first_ch = false;

                                if (is_line_break(ch)) {
                                    line.height += paragraph_spacing;
                                    push_line();
                                    first = word_end;
                                    cont = false;
                                    return false;
                                }

                                bool word_cont{ type >= UBRK_WORD_KANA || type == UBRK_WORD_NONE && is_wide(ch) };
                                if (cont != word_cont) {
                                    push_token();
                                    first = word_begin;
                                    cont = word_cont;
                                }
                            }

                            auto& [spr_x, spr_y, width, advance, left]{ glyphs.at(ch) };
                            if (max_line_length != 0 && cursor != 0 && next_cursor + left + width > max_line_length) {
                                next_cursor -= cursor;
                                push_line(true);
                                first = word_begin;
                            }

                            next_line_width = next_cursor + left + width;
                            next_cursor += advance + letter_spacing;
                            if (is_white_space(ch)) {
                                next_cursor += word_spacing;
                            }
                            if (cont) {
                                ++justified_count;
                            }
                            return true;
                        }
                    )) {
                        cursor = next_cursor;
                        line.width = next_line_width;
                    }
                    last = word_end;

                    return true;
                }
            };

            if (!word_break_for_each(text.str, push_word)) {
                return std::unexpected{ Error::failed_to_tokenize };
            }
            push_line(false, true);

            return Payload{ *_cache.emplace(str, std::move(text)), warning };
        }

        Error text(f64 x, f64 y, const Text& text) const noexcept {
            if (_option.font == nullptr) {
                return Error::font_unspecified;
            }

            auto& layout{ text.layout };

            x += _option.offset_x / _option.scale_x;
            y += _option.offset_y / _option.scale_y + _option.font->top();
            f64 origin_x{ x }, origin_y{ y };

            if (_option.valign == 0) {
                y -= layout.height / 2;
            }
            else if (_option.valign > 0) {
                y -= layout.height;
            }

            static Function draw_sprite_general{ Function::Id::draw_sprite_general };
            u16 height{ _option.font->height() };
            usize spr_id{ _option.font->sprite().id() };
            auto& glyphs{ _option.font->glyphs() };
            f64 cos{ std::cos(_option.rotation) };
            f64 sin{ std::sin(_option.rotation) };
            f64 angle{ -_option.rotation / std::numbers::pi * 180 };
            for (auto& [tokens, line_width, line_height, justified_spacing] : layout.lines) {
                f64 cursor{ x };
                if (_option.halign == 0) {
                    cursor -= line_width / 2;
                }
                else if (_option.halign > 0) {
                    cursor -= line_width;
                }

                for (auto& [str, continuous] : tokens) {
                    unicode_for_each(
                        str,
                        [&](c32 ch) noexcept {
                            auto& [spr_x, spr_y, width, advance, left]{ glyphs.at(ch) };
                            f64 delta_x{ cursor + left - origin_x };
                            f64 delta_y{ y - origin_y };
                            f64 draw_x{ origin_x + delta_x * cos - delta_y * sin };
                            f64 draw_y{ origin_y + delta_y * cos + delta_x * sin };
                            draw_sprite_general(
                                spr_id,
                                0,
                                spr_x,
                                spr_y,
                                width,
                                height,
                                draw_x * _option.scale_x,
                                draw_y * _option.scale_y,
                                _option.scale_x,
                                _option.scale_y,
                                angle,
                                _option.color_top,
                                _option.color_top,
                                _option.color_bottom,
                                _option.color_bottom,
                                _option.alpha
                            );

                            cursor += advance + _option.letter_spacing;
                            if (is_white_space(ch)) {
                                cursor += _option.word_spacing;
                            }
                            if (continuous) {
                                cursor += justified_spacing;
                            }
                            return true;
                        }
                    );
                    if (!continuous) {
                        cursor += justified_spacing;
                    }
                }

                y += line_height;
            }

            return {};
        }
    };

}
