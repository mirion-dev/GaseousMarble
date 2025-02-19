module;

#include <assert.h>

export module gm.draw;

import std;
import gm.core;
import gm.engine;

using namespace gm::core;
using namespace gm::engine;

// image_index == -1 indicates the current subimage
void draw_sprite_general(
    u32 sprite_id,
    i32 image_index,
    u32 image_x,
    u32 image_y,
    u32 width,
    u32 height,
    f64 x,
    f64 y,
    f64 scale_x,
    f64 scale_y,
    f64 rotate,
    u32 color1,
    u32 color2,
    u32 color3,
    u32 color4,
    f64 alpha
) noexcept {
    function[FunctionId::draw_sprite_general].call<void, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real, Real>(
        sprite_id,
        image_index,
        image_x,
        image_y,
        width,
        height,
        x,
        y,
        scale_x,
        scale_y,
        rotate,
        color1,
        color2,
        color3,
        color4,
        alpha
    );
}

u32 sprite_add(
    std::string_view path,
    u32 image_count,
    bool remove_background,
    bool smooth_edges,
    i32 origin_x,
    i32 origin_y
) noexcept {
    return function[FunctionId::sprite_add].call<u32, String, Real, Real, Real, Real, Real>(
        path,
        image_count,
        remove_background,
        smooth_edges,
        origin_x,
        origin_y
    );
}

void sprite_delete(u32 sprite_id) noexcept {
    function[FunctionId::sprite_delete].call<void, Real>(sprite_id);
}

class SpriteHandle {
    u32 _id;

public:
    SpriteHandle() noexcept :
        _id{} {}

    SpriteHandle(std::nullptr_t) noexcept :
        _id{} {}

    SpriteHandle(u32 id) noexcept :
        _id{ id } {}

    operator bool() const noexcept {
        return _id != 0;
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

    void operator()(SpriteHandle handle) const noexcept {
        sprite_delete(handle.id());
    }
};

export namespace gm::draw {

    struct GlyphData {
        u16 x, y;
        u16 width;
        i16 left;
    };

    class Font {
        static inline u32 _counter;

        u32 _id;
        u16 _size;
        u16 _height;
        std::unique_ptr<SpriteHandle, SpriteDeleter> _sprite;
        std::unordered_map<u32, GlyphData> _glyph;

    public:
        Font() noexcept :
            _id{},
            _size{},
            _height{},
            _sprite{} {}

        Font(std::string_view sprite_path, std::string_view glyph_path) noexcept :
            _id{ ++_counter },
            _size{},
            _height{},
            _sprite{ sprite_add(sprite_path, 1, false, false, 0, 0) } {

            std::ifstream file{ glyph_path.data(), std::ios::binary };
            if (!file.is_open()) {
                return;
            }

            char magic[4];
            file.read(magic, sizeof(magic));
            if (std::strncmp(magic, "GLY", sizeof(magic)) != 0) {
                return;
            }

            file.read(reinterpret_cast<char*>(&_size), sizeof(_size));
            file.read(reinterpret_cast<char*>(&_height), sizeof(_height));

            while (file) {
                wchar_t ch;
                file.read(reinterpret_cast<char*>(&ch), sizeof(ch));
                file.read(reinterpret_cast<char*>(&_glyph[ch]), sizeof(_glyph[ch]));
            }
        }

        operator bool() const noexcept {
            return _id != 0;
        }

        bool operator==(const Font& other) const noexcept {
            return _id == other.id();
        }

        u32 id() const noexcept {
            return _id;
        }

        u16 size() const noexcept {
            assert(_id != 0);
            return _size;
        }

        u16 height() const noexcept {
            assert(_id != 0);
            return _height;
        }

        SpriteHandle sprite() const noexcept {
            assert(_id != 0);
            return _sprite.get();
        }

        const auto& glyph() const noexcept {
            assert(_id != 0);
            return _glyph;
        }
    };

    struct DrawSetting {
        Font* font;
        u32 color_top, color_bottom;
        f64 alpha;
        i8 halign, valign;
        f64 word_spacing, letter_spacing;
        f64 max_line_width;
        f64 line_height;
        f64 offset_x, offset_y;
        f64 scale_x, scale_y;
    };

    class Draw {
        DrawSetting _setting;

        std::u32string _filter(std::u32string_view text) const noexcept {
            std::u32string filtered;
            auto& glyph_map{ _setting.font->glyph() };

            for (auto& ch : text) {
                if (std::iswblank(ch)) {
                    filtered += ' ';
                }
                else if (ch == '\n' || !std::iswcntrl(ch) && glyph_map.find(ch) != glyph_map.end()) {
                    filtered += ch;
                }
            }
            return filtered;
        }

        auto _split(std::u32string_view text) const noexcept {
            std::vector<std::pair<std::u32string, f64>> line;
            auto& glyph_map{ _setting.font->glyph() };
            f64 max_line_width{ _setting.max_line_width * _setting.scale_x };
            f64 word_spacing{ _setting.word_spacing * _setting.scale_x };
            f64 letter_spacing{ _setting.letter_spacing * _setting.scale_x };

            auto begin{ text.begin() }, end{ text.end() }, i{ begin };
            f64 line_width{}, last_spacing{};
            while (i != end) {
                if (*i == '\n') {
                    line.emplace_back(std::u32string{ begin, i }, line_width - last_spacing);
                    line_width = last_spacing = 0;
                    begin = ++i;
                    continue;
                }

                f64 spacing{ letter_spacing };
                if (*i == ' ') {
                    spacing += word_spacing;
                }
                auto& glyph{ glyph_map.at(*i) };
                f64 char_width{ (glyph.left + glyph.width) * _setting.scale_x + spacing };

                if (max_line_width == 0 || line_width + char_width - spacing <= max_line_width) {
                    line_width += char_width;
                }
                else {
                    line.emplace_back(std::u32string{ begin, i }, line_width - last_spacing);
                    line_width = char_width;
                    begin = i;
                }
                last_spacing = spacing;
                ++i;
            }
            line.emplace_back(std::u32string{ begin, end }, line_width - last_spacing);
            return line;
        }

        void _glyph(f64 x, f64 y, const GlyphData& glyph) const noexcept {
            draw_sprite_general(
                _setting.font->sprite().id(),
                0,
                glyph.x,
                glyph.y,
                glyph.width,
                _setting.font->height(),
                x + glyph.left * _setting.scale_x,
                y,
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
            f64 word_spacing{ _setting.word_spacing * _setting.scale_x };
            f64 letter_spacing{ _setting.letter_spacing * _setting.scale_x };

            for (auto& ch : text) {
                auto& glyph{ glyph_map.at(ch) };
                _glyph(x, y, glyph);
                x += (glyph.left + glyph.width) * _setting.scale_x + letter_spacing;
                if (ch == ' ') {
                    x += word_spacing;
                }
            }
        }

    public:
        Draw() noexcept {
            _setting.font = nullptr;
            _setting.color_top = _setting.color_bottom = 0xffffff;
            _setting.alpha = 1;
            _setting.halign = _setting.valign = -1;
            _setting.word_spacing = _setting.letter_spacing = 0;
            _setting.max_line_width = 0;
            _setting.line_height = 1;
            _setting.offset_x = _setting.offset_y = 0;
            _setting.scale_x = _setting.scale_y = 1;
        }

        DrawSetting& setting() noexcept {
            return _setting;
        }

        f64 width(std::u32string_view text) const noexcept {
            auto line{ _split(_filter(text)) };
            f64 max_width{};
            for (auto& [text, width] : line) {
                max_width = std::max(max_width, width);
            }
            return max_width;
        }

        f64 height(std::u32string_view text) const noexcept {
            auto line{ _split(_filter(text)) };
            f64 line_height{ _setting.line_height * _setting.scale_y * _setting.font->size() };
            return line_height * line.size();
        }

        bool text(f64 x, f64 y, std::u32string_view text) const noexcept {
            if (_setting.font == nullptr) {
                return false;
            }

            std::u32string filtered{ _filter(text) };
            auto line{ _split(filtered) };
            f64 line_height{ _setting.line_height * _setting.scale_y * _setting.font->size() };
            f64 offset_x{ _setting.offset_x * _setting.scale_x };
            f64 offset_y{ _setting.offset_y * _setting.scale_y };

            x += offset_x;
            y += offset_y;
            if (_setting.valign < 0) {
                // do nothing
            }
            if (_setting.valign == 0) {
                y -= line_height * line.size() / 2;
            }
            else {
                y -= line_height * line.size();
            }

            if (_setting.halign < 0) {
                for (auto& [text, width] : line) {
                    _line(x, y, text);
                    y += line_height;
                }
            }
            else if (_setting.halign == 0) {
                for (auto& [text, width] : line) {
                    _line(x - width / 2, y, text);
                    y += line_height;
                }
            }
            else {
                for (auto& [text, width] : line) {
                    _line(x - width, y, text);
                    y += line_height;
                }
            }

            return text.size() == filtered.size();
        }
    };

}