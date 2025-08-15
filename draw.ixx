module;

#include <cassert>
#include <icu.h>

export module gm:draw;

import std;
import :core;
import :engine;

namespace gm {

    export class SpriteHandle {
        static constexpr auto NULL_ID{ static_cast<u32>(-1) };

        u32 _id{ NULL_ID };

    public:
        SpriteHandle() noexcept = default;

        SpriteHandle(std::nullptr_t) noexcept {};

        SpriteHandle(u32 id) noexcept :
            _id{ id } {}

        operator bool() const noexcept {
            return _id != NULL_ID;
        }

        bool operator==(SpriteHandle other) const noexcept {
            return _id == other._id;
        }

        u32 id() const noexcept {
            return _id;
        }
    };

    export struct SpriteDeleter {
        using pointer = SpriteHandle;

        void operator()(pointer handle) const noexcept {
            IFunctionResource::at(FunctionId::sprite_delete)(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        u16 advance;
        i16 left;
    };

    export class InvalidHeaderError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    export class DataCorruptionError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

    export class Font {
        u16 _height;
        i16 _top;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<u32, GlyphData> _glyph_map;

    public:
        Font() noexcept = default;

        Font(std::string_view font_name, std::string_view sprite_path) {
            auto glyph_path{ std::string{ sprite_path.substr(0, sprite_path.find_last_of('.')) } + ".gly" };
            std::ifstream file{ glyph_path, std::ios::binary };
            if (!file.is_open()) {
                throw std::ios_base::failure{ std::format("Unable to open the file {}.", glyph_path) };
            }

            static constexpr char GLYPH_SIGN[]{ "GLY\x00\x12\x00" };
            char sign[sizeof(GLYPH_SIGN) - 1];
            file.read(sign, sizeof(sign));
            if (!file || std::strncmp(sign, GLYPH_SIGN, sizeof(sign)) != 0) {
                throw InvalidHeaderError{ std::format("Invalid file header in {}.", glyph_path) };
            }

            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
            file.read(reinterpret_cast<char*>(&_top), sizeof(_top));
            while (!file.eof()) {
                u32 ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph_map[ch]), sizeof(_glyph_map[ch]));
                if (!file) {
                    throw DataCorruptionError{ std::format("File {} is corrupt.", glyph_path) };
            }
            }

            _name = font_name;
            _sprite.reset(
                static_cast<u32>(IFunctionResource::at(FunctionId::sprite_add)(sprite_path, 1, false, false, 0, 0))
            );
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

        const auto& glyph_map() const noexcept {
            assert(_sprite != nullptr);
            return _glyph_map;
        }
    };

    export struct DrawSetting {
        Font* font{};
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f64 alpha{ 1 };
        i8 halign{ -1 };
        i8 valign{ -1 };
        bool justified{};
        f64 letter_spacing{};
        f64 word_spacing{};
        f64 paragraph_spacing{};
        f64 line_height{ 1 };
        f64 max_line_length{};
        f64 offset_x{};
        f64 offset_y{};
        f64 scale_x{ 1 };
        f64 scale_y{ 1 };
    };

    export class Draw {
        DrawSetting _setting;

        struct MeasureResult {
            struct Line {
                std::u32string text;
                f64 width;
                f64 height;
                f64 letter_spacing;
            };

            std::vector<Line> lines;
            f64 total_width{};
            f64 total_height{};
        };

        MeasureResult _measure(std::string_view text) const noexcept {
            auto& glyph_map{ _setting.font->glyph_map() };
            const char* str{ text.data() };
            i32 j{}, n{ static_cast<i32>(text.size()) }, ch;
            std::u32string filtered_text;
            filtered_text.reserve(n);
            while (j != n) {
                U8_NEXT_OR_FFFD(str, j, n, ch);
                if (ch == 0xfffd) {
                    break;
                }
                if (ch == '\n' || glyph_map.contains(ch)) {
                    filtered_text.push_back(ch);
                }
            }

            MeasureResult result;

            f64 max_line_length{ _setting.max_line_length / _setting.scale_x };
            auto add_line{
                   // @formatter:off
                [this, max_line_length, &result](
                    std::u32string_view text,
                    f64 line_length,
                    f64 is_full,
                    f64 is_paragraph_end
                ) {
                    // @formatter:on
                    f64 width{ line_length };
                    f64 letter_spacing{ _setting.letter_spacing };
                    if (_setting.justified && max_line_length != 0 && is_full && text.size() != 1) {
                        width = max_line_length;
                        letter_spacing += (max_line_length - line_length) / (text.size() - 1);
                    }

                    f64 height{ _setting.font->height() * _setting.line_height };
                    if (is_paragraph_end) {
                        height += _setting.paragraph_spacing;
                    }

                    result.lines.emplace_back(std::u32string{ text }, width, height, letter_spacing);
                    result.total_width = std::max(result.total_width, width);
                    result.total_height += height;
                }
            };

            f64 line_length{}, last_spacing{};
            auto begin{ filtered_text.begin() }, end{ filtered_text.end() }, i{ begin };
            while (i != end) {
                if (*i != '\n') {
                    auto& [glyph_x, glyph_y, glyph_width, offset_x]{ glyph_map.at(*i) };

                    f64 right{ static_cast<f64>(offset_x + glyph_width) };
                    f64 spacing{ _setting.letter_spacing };
                    if (*i == ' ') {
                        spacing += _setting.word_spacing;
                    }

                    if (max_line_length != 0 && line_length + right > max_line_length) {
                        add_line({ begin, i }, line_length - last_spacing, true, false);
                        begin = i;
                        line_length = 0;
                    }

                    line_length += right + spacing;
                    last_spacing = spacing;
                    ++i;
                }
                else {
                    add_line({ begin, i }, line_length - last_spacing, false, true);
                    begin = ++i;
                    line_length = last_spacing = 0;
                }
            }
            add_line({ begin, i }, line_length - last_spacing, false, false);

            return result;
        }

        void _line(f64 x, f64 y, std::u32string_view text, f64 letter_spacing) const noexcept {
            auto& glyph_map{ _setting.font->glyph_map() };
            u16 glyph_height{ _setting.font->height() };
            u32 font_id{ _setting.font->sprite().id() };

            for (u32 ch : text) {
                auto& [glyph_x, glyph_y, glyph_width, offset_x]{ glyph_map.at(ch) };

                IFunctionResource::at(FunctionId::draw_sprite_general)(
                    font_id,
                    0,
                    glyph_x,
                    glyph_y,
                    glyph_width,
                    glyph_height,
                    (x + offset_x) * _setting.scale_x,
                    y * _setting.scale_y,
                    _setting.scale_x,
                    _setting.scale_y,
                    0,
                    _setting.color_top,
                    _setting.color_top,
                    _setting.color_bottom,
                    _setting.color_bottom,
                    _setting.alpha
                );

                x += offset_x + glyph_width + letter_spacing;
                if (ch == ' ') {
                    x += _setting.word_spacing;
                }
            }
        }

    public:
        DrawSetting setting;

        f64 width(std::string_view text) const noexcept {
            return _measure(text).total_width * _setting.scale_x;
        }

        f64 height(std::string_view text) const noexcept {
            return _measure(text).total_height * _setting.scale_y;
        }

        bool text(f64 x, f64 y, std::string_view text) const noexcept {
            if (_setting.font == nullptr) {
                return false;
            }

            MeasureResult result{ _measure(text) };

            x += _setting.offset_x / _setting.scale_x;
            y += (_setting.offset_y + _setting.font->offset_y()) / _setting.scale_y;

            if (_setting.valign == 0) {
                y -= result.total_height / 2;
            }
            else if (_setting.valign > 0) {
                y -= result.total_height;
            }

            for (auto& [text, width, height, letter_spacing] : result.lines) {
                f64 real_x{ x };
                if (_setting.halign == 0) {
                    real_x -= width / 2;
                }
                else if (_setting.halign > 0) {
                    real_x -= width;
                }

                _line(real_x, y, text, letter_spacing);

                y += height;
            }

            return true;
        }
    };

}
