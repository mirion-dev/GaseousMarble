module;

#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;
import :font;

namespace gm {

    export class Draw {
    public:
        struct Setting {
            Font* font{};
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

        struct Token {
            std::u16string_view text;
            bool continuous;
        };

        struct LineLayout {
            std::vector<Token> tokens;
            f64 width;
            f64 height;
            f64 justified_spacing;
        };

        struct TextLayout {
            std::vector<LineLayout> lines;
            f64 width;
            f64 height;
        };

        struct Text {
            std::u16string text;
            TextLayout layout;
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

        Setting setting;

        Result<Text, Warning, Error> create_text(std::u8string_view text) const noexcept {
            auto& glyphs{ setting.font->glyphs() };

            std::u16string utf16;
            Warning warning{};
            bool ok{};
            utf16.resize_and_overwrite(
                text.size(),
                [&](c16* ptr, usize) noexcept {
                    usize size{};
                    ok = unicode_for_each(
                        text,
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

            f64 max_line_length{ setting.max_line_length / setting.scale_x };
            f64 line_height{ setting.font->height() * setting.line_height };

            TextLayout layout;

            const c16* ptr{ utf16.data() };
            usize size{};
            bool cont{};

            LineLayout line{ .height = line_height };
            f64 cursor{};
            usize justified_count{};

            // update `line`, `justified_count` and reset `size`
            auto push_token{
                [&] noexcept {
                    if (size == 0) {
                        return;
                    }
                    if (!cont) {
                        ++justified_count;
                    }
                    line.tokens.emplace_back(std::u16string_view{ ptr, size }, cont);
                    size = 0;
                }
            };

            // update `metrics` and reset `size`, `line`, `x`, `justified_count`
            auto push_line{
                [&](bool auto_wrap = false) noexcept {
                    push_token();

                    if (auto_wrap && setting.justified && max_line_length != 0 && justified_count > 1) {
                        line.justified_spacing = (max_line_length - line.width) / (justified_count - 1);
                        line.width = max_line_length;
                    }

                    layout.lines.emplace_back(std::move(line));
                    layout.width = std::max(layout.width, line.width);
                    layout.height += line.height;

                    line = { .height = line_height };
                    cursor = 0;
                    justified_count = 0;
                }
            };

            ok = word_break_for_each(
                utf16,
                [&](std::u16string_view word, i32 type) noexcept {
                    const c16* word_ptr{ word.data() };
                    usize word_size{ word.size() }, i{};
                    i32 ch;
                    U16_NEXT_UNSAFE(word_ptr, i, ch);
                    if (is_line_break(ch)) {
                        line.height += setting.paragraph_spacing;
                        push_line();
                        ptr = word_ptr + word_size;
                        cont = false;
                        return true;
                    }

                    bool word_cont{ type >= UBRK_WORD_KANA || type == UBRK_WORD_NONE && is_wide(ch) };
                    if (cont != word_cont) {
                        push_token();
                        ptr = word_ptr;
                        cont = word_cont;
                    }

                    f64 next_cursor{ cursor }, next_line_width;
                    while (true) {
                        auto& [spr_x, spr_y, width, advance, left]{ glyphs.at(ch) };
                        if (max_line_length != 0 && next_cursor + left + width > max_line_length && cursor != 0) {
                            next_cursor -= cursor;
                            push_line(true);
                            ptr = word_ptr;
                        }

                        next_line_width = next_cursor + left + width;
                        next_cursor += advance + setting.letter_spacing;
                        if (is_white_space(ch)) {
                            next_cursor += setting.word_spacing;
                        }
                        if (cont) {
                            ++justified_count;
                        }

                        if (i == word_size) {
                            break;
                        }
                        U16_NEXT_UNSAFE(word_ptr, i, ch);
                    }

                    size += word_size;
                    cursor = next_cursor;
                    line.width = next_line_width;
                    if (next_line_width > max_line_length) {
                        push_line();
                        ptr = word_ptr + word_size;
                    }
                    return true;
                }
            );
            if (!ok) {
                return std::unexpected{ Error::failed_to_tokenize };
            }
            push_line();

            return Payload{ Text{ std::move(utf16), std::move(layout) }, warning };
        }

        Error text(f64 x, f64 y, const Text& text) const noexcept {
            if (setting.font == nullptr) {
                return Error::font_unspecified;
            }

            auto& [_, layout]{ text };

            x += setting.offset_x / setting.scale_x;
            y += setting.offset_y / setting.scale_y + setting.font->top();
            f64 origin_x{ x }, origin_y{ y };

            if (setting.valign == 0) {
                y -= layout.height / 2;
            }
            else if (setting.valign > 0) {
                y -= layout.height;
            }

            static Function draw_sprite_general{ Function::Id::draw_sprite_general };
            auto& glyphs{ setting.font->glyphs() };
            usize spr_id{ setting.font->sprite().id() };
            u16 height{ setting.font->height() };
            f64 cos{ std::cos(setting.rotation) }, sin{ std::sin(setting.rotation) };
            for (auto& [tokens, line_width, line_height, justified_spacing] : layout.lines) {
                f64 cursor{ x };
                if (setting.halign == 0) {
                    cursor -= line_width / 2;
                }
                else if (setting.halign > 0) {
                    cursor -= line_width;
                }

                for (auto& [text, cont] : tokens) {
                    const c16* ptr{ text.data() };
                    c32 ch;
                    for (usize i{}, size{ text.size() }; i != size;) {
                        U16_NEXT_UNSAFE(ptr, i, ch);
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
                            draw_x * setting.scale_x,
                            draw_y * setting.scale_y,
                            setting.scale_x,
                            setting.scale_y,
                            -setting.rotation / std::numbers::pi * 180,
                            setting.color_top,
                            setting.color_top,
                            setting.color_bottom,
                            setting.color_bottom,
                            setting.alpha
                        );

                        cursor += advance + setting.letter_spacing;
                        if (is_white_space(ch)) {
                            cursor += setting.word_spacing;
                        }
                        if (cont) {
                            cursor += justified_spacing;
                        }
                    }
                    if (!cont) {
                        cursor += justified_spacing;
                    }
                }

                y += line_height;
            }

            return {};
        }
    };

}
