module;

#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;
import :font;

namespace gm {

    export class Text {
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

        enum class Error {
            no_error           = 0,
            invalid_encoding   = -1,
            failed_to_tokenize = -2,
            font_unspecified   = -3
        };

    private:
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

        std::u16string _str;
        Layout _layout{};

    public:
        Text() noexcept = default;

        Text(std::u8string_view str, const Option& option) {
            if (option.font == nullptr) {
                throw Error::font_unspecified;
            }

            auto& glyphs{ option.font->glyphs() };
            auto glyph_height{ static_cast<f64>(option.font->height()) };
            bool justified{ option.justified };
            f64 letter_spacing{ option.letter_spacing };
            f64 word_spacing{ option.word_spacing };
            f64 paragraph_spacing{ option.paragraph_spacing };
            f64 line_height{ option.line_height };
            f64 max_line_length{ std::max(option.max_line_length, 0.) / option.scale_x };

            bool ok{};
            _str.resize_and_overwrite(
                str.size(),
                [&](c16* ptr, usize) noexcept {
                    usize size{};
                    ok = unicode_for_each(
                        str,
                        [&](c32 ch) noexcept {
                            if (glyphs.contains(ch) || is_line_break(ch)) {
                                U16_APPEND_UNSAFE(ptr, size, ch);
                            }
                            return true;
                        }
                    );
                    return size;
                }
            );
            if (!ok) {
                throw Error::invalid_encoding;
            }

            const c16* first{ _str.data() };
            const c16* last{ first };
            bool cont{};

            Line line{};
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

                    if (auto_wrap && justified && justified_count > 1) {
                        line.justified_spacing = (max_line_length - line.width) / (justified_count - 1);
                        line.width = max_line_length;
                    }

                    line.height = glyph_height;
                    if (!last) {
                        line.height *= line_height;
                    }

                    _layout.lines.emplace_back(std::move(line));
                    _layout.width = std::max(_layout.width, line.width);
                    _layout.height += line.height;

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

            if (!word_break_for_each(_str, push_word)) {
                throw Error::failed_to_tokenize;
            }
            push_line(false, true);
        }

        std::expected<void, Error> draw(f64 x, f64 y, const Option& option) const noexcept {
            if (option.font == nullptr) {
                return std::unexpected{ Error::font_unspecified };
            }

            x += option.offset_x / option.scale_x;
            y += option.offset_y / option.scale_y + option.font->top();
            f64 origin_x{ x }, origin_y{ y };

            if (option.valign == 0) {
                y -= _layout.height / 2;
            }
            else if (option.valign > 0) {
                y -= _layout.height;
            }

            static Function draw_sprite_general{ Function::Id::draw_sprite_general };
            u16 height{ option.font->height() };
            usize spr_id{ option.font->sprite().id() };
            auto& glyphs{ option.font->glyphs() };
            f64 radian{ -option.rotation / 180 * std::numbers::pi };
            f64 cos{ std::cos(radian) };
            f64 sin{ std::sin(radian) };
            for (auto& [tokens, line_width, line_height, justified_spacing] : _layout.lines) {
                f64 cursor{ x };
                if (option.halign == 0) {
                    cursor -= line_width / 2;
                }
                else if (option.halign > 0) {
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
                                draw_x * option.scale_x,
                                draw_y * option.scale_y,
                                option.scale_x,
                                option.scale_y,
                                option.rotation,
                                option.color_top,
                                option.color_top,
                                option.color_bottom,
                                option.color_bottom,
                                option.alpha
                            );

                            cursor += advance + option.letter_spacing;
                            if (is_white_space(ch)) {
                                cursor += option.word_spacing;
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

        f64 width() const noexcept {
            return std::abs(_layout.width);
        }

        f64 height() const noexcept {
            return std::abs(_layout.height);
        }
    };

}
