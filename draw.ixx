module;

#include <cassert>

export module gm:draw;

import std;
import :core;
import :engine;

// Font
namespace gm {

    class SpriteHandle {
        static constexpr auto _null_id{ static_cast<u32>(-1) };

        u32 _id{ _null_id };

    public:
        SpriteHandle() noexcept = default;

        SpriteHandle(std::nullptr_t) noexcept {};

        SpriteHandle(u32 id) noexcept :
            _id{ id } {}

        operator bool() const noexcept {
            return _id != _null_id;
        }

        bool operator==(SpriteHandle other) const noexcept {
            return _id == other._id;
        }

        u32 id() const noexcept {
            return _id;
        }
    };

    struct SpriteDeleter {
        using pointer = SpriteHandle;

        void operator()(pointer handle) const noexcept {
            IFunctionResource::at(FunctionId::sprite_delete).call<void, Real>(handle.id());
        }
    };

    export struct GlyphData {
        u16 x, y;
        u16 width;
        i16 left;
    };

    export class Font {
        u16 _size;
        u16 _height;
        std::string _name;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<u32, GlyphData> _glyph;

    public:
        Font() noexcept = default;

        Font(std::string_view name, std::string_view sprite_path, std::string_view glyph_path) noexcept :
            _name{ name } {

            std::ifstream file{ glyph_path.data(), std::ios::binary };
            if (!file) {
                return;
            }

            char magic[4];
            file.read(magic, sizeof(magic));
            if (std::strncmp(magic, "GLY", sizeof(magic)) != 0) {
                return;
            }

            _sprite.reset(
                IFunctionResource::at(FunctionId::sprite_add)
                .call<u32, String, Real, Real, Real, Real, Real>(
                    sprite_path,
                    1,
                    false,
                    false,
                    0,
                    0
                )
            );

            file.read(reinterpret_cast<char*>(&_size), sizeof(_size));
            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));
            while (file) {
                u16 ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph[ch]), sizeof(_glyph[ch]));
            }
        }

        operator bool() const noexcept {
            return _sprite != nullptr;
        }

        bool operator==(const Font& other) const noexcept {
            return _sprite == other._sprite;
        }

        u16 size() const noexcept {
            assert(_sprite);
            return _size;
        }

        u16 height() const noexcept {
            assert(_sprite);
            return _height;
        }

        const std::string& name() const noexcept {
            assert(_sprite);
            return _name;
        }

        SpriteHandle sprite() const noexcept {
            assert(_sprite);
            return _sprite.get();
        }

        const auto& glyph() const noexcept {
            assert(_sprite);
            return _glyph;
        }
    };

}

// Draw
namespace gm {

    export struct DrawSetting {
        Font* font{};
        u32 color_top{ 0xffffff };
        u32 color_bottom{ 0xffffff };
        f64 alpha{ 1 };
        i8 halign{ -1 };
        i8 valign{ -1 };
        f64 word_spacing{};
        f64 letter_spacing{};
        f64 max_line_width{};
        f64 line_height{ 1 };
        f64 offset_x{};
        f64 offset_y{};
        f64 scale_x{ 1 };
        f64 scale_y{ 1 };
    };

    export class Draw {
        DrawSetting _setting;

        std::u32string _filter(std::string_view text) const noexcept {
            auto& glyph_map{ _setting.font->glyph() };
            return std::ranges::to<std::u32string>(
                utf8_decode(text)
                | std::views::filter(
                    [&](u32 ch) {
                        return ch == '\n' || ch >= ' ' && ch != '\x7f' && glyph_map.contains(ch);
                    }
                )
            );
        }

        auto _split(std::u32string_view text) const noexcept {
            std::vector<std::pair<std::u32string, f64>> lines;

            auto& glyph_map{ _setting.font->glyph() };
            f64 max_line_width{ _setting.max_line_width / _setting.scale_x };
            f64 line_width{}, last_spacing{};
            auto begin{ text.begin() }, end{ text.end() }, i{ begin };
            while (i != end) {
                if (*i == '\n') {
                    lines.emplace_back(std::u32string{ begin, i }, line_width - last_spacing);
                    line_width = last_spacing = 0;
                    begin = ++i;
                }
                else {
                    auto& glyph{ glyph_map.at(*i) };
                    f64 spacing{ _setting.letter_spacing };
                    if (*i == ' ') {
                        spacing += _setting.word_spacing;
                    }
                    f64 char_width{ glyph.left + glyph.width + spacing };

                    if (_setting.max_line_width == 0 || line_width + glyph.left + glyph.width <= max_line_width) {
                        line_width += char_width;
                    }
                    else {
                        lines.emplace_back(std::u32string{ begin, i }, line_width - last_spacing);
                        line_width = char_width;
                        begin = i;
                    }
                    last_spacing = spacing;
                    ++i;
                }
            }
            lines.emplace_back(std::u32string{ begin, end }, line_width - last_spacing);

            return lines;
        }

        void _glyph(f64 x, f64 y, const GlyphData& glyph) const noexcept {
            IFunctionResource::at(FunctionId::draw_sprite_general)
                .call<void, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real>(
                    _setting.font->sprite().id(),
                    0,
                    glyph.x,
                    glyph.y,
                    glyph.width,
                    _setting.font->height(),
                    (x + glyph.left) * _setting.scale_x,
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
        }

        void _line(f64 x, f64 y, std::u32string_view text) const noexcept {
            auto& glyph_map{ _setting.font->glyph() };
            for (u32 ch : text) {
                auto& glyph{ glyph_map.at(ch) };
                _glyph(x, y, glyph);

                x += glyph.left + glyph.width + _setting.letter_spacing;
                if (ch == ' ') {
                    x += _setting.word_spacing;
                }
            }
        }

    public:
        auto&& setting(this auto&& self) noexcept {
            return std::forward_like<decltype(self)>(self._setting);
        }

        f64 width(std::string_view text) const noexcept {
            return std::ranges::max(_split(_filter(text)) | std::views::values) * _setting.scale_x;
        }

        f64 height(std::string_view text) const noexcept {
            return _setting.line_height * _setting.font->size() * _split(_filter(text)).size() * _setting.scale_y;
        }

        bool text(f64 x, f64 y, std::string_view text) const noexcept {
            if (_setting.font == nullptr) {
                return false;
            }

            auto lines{ _split(_filter(text)) };

            x += _setting.offset_x;
            y += _setting.offset_y;

            f64 line_height{ _setting.line_height * _setting.font->size() };
            if (_setting.valign == 0) {
                y -= line_height * lines.size() / 2;
            }
            else if (_setting.valign > 0) {
                y -= line_height * lines.size();
            }

            if (_setting.halign < 0) {
                for (auto& text : std::views::keys(lines)) {
                    _line(x, y, text);
                    y += line_height;
                }
            }
            else if (_setting.halign == 0) {
                for (auto& [text, width] : lines) {
                    _line(x - width / 2, y, text);
                    y += line_height;
                }
            }
            else {
                for (auto& [text, width] : lines) {
                    _line(x - width, y, text);
                    y += line_height;
                }
            }

            return true;
        }
    };

}
