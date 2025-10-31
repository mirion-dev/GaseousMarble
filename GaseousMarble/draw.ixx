module;

#include <cassert>
#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;

namespace gm {

    export class SpriteHandle {
        static constexpr i32 NULL_ID{ -1 };

        i32 _id{ NULL_ID };

    public:
        SpriteHandle() noexcept = default;

        SpriteHandle(std::nullptr_t) noexcept {};

        SpriteHandle(i32 id) noexcept :
            _id{ id } {}

        operator bool() const noexcept {
            return _id != NULL_ID;
        }

        bool operator==(SpriteHandle other) const noexcept {
            return _id == other._id;
        }

        i32 id() const noexcept {
            return _id;
        }
    };

    export struct SpriteDeleter {
        using pointer = SpriteHandle;

        void operator()(pointer handle) const noexcept {
            static Function sprite_delete{ Function::Id::sprite_delete };
            sprite_delete(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        i16 advance;
        i16 left;
    };

    export class Font {
        u16 _height;
        i16 _top;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<i32, GlyphData> _glyph_data;

    public:
        enum class Error {
            failed_to_open_file  = -1,
            invalid_header       = -2,
            data_corrupted       = -3,
            failed_to_add_sprite = -4
        };

        Font() noexcept = default;

        Font(std::string_view font_name, std::string_view sprite_path) {
            auto glyph_path{ std::string{ sprite_path.substr(0, sprite_path.find_last_of('.')) } + ".gly" };
            std::ifstream file{ glyph_path, std::ios::binary };
            if (!file.is_open()) {
                throw Error::failed_to_open_file;
            }

            static constexpr char GLYPH_SIGN[]{ 'G', 'L', 'Y', 1, 0, 0 };
            char sign[sizeof(GLYPH_SIGN)];
            file.read(sign, sizeof(sign));
            if (!file || !std::ranges::equal(sign, GLYPH_SIGN)) {
                throw Error::invalid_header;
            }

            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
            file.read(reinterpret_cast<char*>(&_top), sizeof(_top));
            while (file) {
                u32 ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph_data[ch]), sizeof(_glyph_data[ch]));
            }
            if (!file.eof()) {
                throw Error::data_corrupted;
            }

            _name = font_name;

            static Function sprite_add{ Function::Id::sprite_add };
            _sprite.reset(static_cast<i32>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            if (_sprite == nullptr) {
                throw Error::failed_to_add_sprite;
            }
        }

        operator bool() const noexcept {
            return _sprite != nullptr;
        }

        bool operator==(const Font& other) const noexcept {
            return _sprite == other._sprite;
        }

        u16 height() const noexcept {
            assert(_sprite != nullptr);
            return _height;
        }

        i16 top() const noexcept {
            assert(_sprite != nullptr);
            return _top;
        }

        const std::string& name() const noexcept {
            assert(_sprite != nullptr);
            return _name;
        }

        SpriteHandle sprite() const noexcept {
            assert(_sprite != nullptr);
            return _sprite.get();
        }

        const auto& glyph_data() const noexcept {
            assert(_sprite != nullptr);
            return _glyph_data;
        }
    };

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

        struct LineMetrics {
            std::vector<Token> tokens;
            f64 width;
            f64 height;
            f64 justified_spacing;
        };

        struct TextMetrics {
            std::u16string text;
            std::vector<LineMetrics> lines;
            f64 width;
            f64 height;
        };

        enum class Warning {
            no_warning     = 0,
            missing_glyphs = 1
        };

        enum class Error {
            invalid_encoding   = -1,
            failed_to_tokenize = -2,
            font_unspecified   = -3
        };

        Setting setting;

        Result<TextMetrics, Warning, Error> measure(std::string_view text) const noexcept {
            auto& glyph_data{ setting.font->glyph_data() };

            std::u16string utf16;
            Warning warning{};
            bool ok{};
            utf16.resize_and_overwrite(
                text.size(),
                [&](char16_t* ptr, u32) noexcept {
                    u32 size{};
                    ok = unicode_for_each(
                        text,
                        [&](i32 ch) noexcept {
                            if (glyph_data.contains(ch) || is_line_break(ch)) {
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

            TextMetrics metrics{ std::move(utf16) };

            const char16_t* ptr{ metrics.text.data() };
            u32 size{};
            bool cont{};

            LineMetrics line{ .height = line_height };
            f64 cursor{};
            u32 justified_count{};

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

                    metrics.lines.emplace_back(std::move(line));
                    metrics.width = std::max(metrics.width, line.width);
                    metrics.height += line.height;

                    line = { .height = line_height };
                    cursor = 0;
                    justified_count = 0;
                }
            };

            ok = word_break_for_each(
                metrics.text,
                [&](std::u16string_view word, i32 type) noexcept {
                    const char16_t* word_ptr{ word.data() };
                    u32 word_size{ word.size() }, i{};
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
                        auto& [spr_x, spr_y, width, advance, left]{ glyph_data.at(ch) };
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

            return Wrapped{ std::move(metrics), warning };
        }

        Result<void, Warning, Error> text(f64 x, f64 y, std::string_view text) const noexcept {
            if (setting.font == nullptr) {
                return std::unexpected{ Error::font_unspecified };
            }

            auto exp_metrics{ measure(text) };
            if (!exp_metrics) {
                return std::unexpected{ exp_metrics.error() };
            }
            auto [metrics, warning]{ std::move(*exp_metrics) };

            x += setting.offset_x / setting.scale_x;
            y += setting.offset_y / setting.scale_y + setting.font->top();
            f64 origin_x{ x }, origin_y{ y };

            if (setting.valign == 0) {
                y -= metrics.height / 2;
            }
            else if (setting.valign > 0) {
                y -= metrics.height;
            }

            static Function draw_sprite_general{ Function::Id::draw_sprite_general };
            auto& glyph_data{ setting.font->glyph_data() };
            i32 spr_id{ setting.font->sprite().id() };
            u16 height{ setting.font->height() };
            f64 cos{ std::cos(setting.rotation) }, sin{ std::sin(setting.rotation) };
            for (auto& [tokens, line_width, line_height, justified_spacing] : metrics.lines) {
                f64 cursor{ x };
                if (setting.halign == 0) {
                    cursor -= line_width / 2;
                }
                else if (setting.halign > 0) {
                    cursor -= line_width;
                }

                for (auto& [text, cont] : tokens) {
                    const char16_t* ptr{ text.data() };
                    i32 ch;
                    for (u32 i{}, size{ text.size() }; i != size;) {
                        U16_NEXT_UNSAFE(ptr, i, ch);
                        auto& [spr_x, spr_y, width, advance, left]{ glyph_data.at(ch) };

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

            return Wrapped{ warning };
        }
    };

}
