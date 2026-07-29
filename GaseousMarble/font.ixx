module;

#include <cassert>

export module gm.font;

import std;
import gm.types;
import gm.utils;
import gm.env;

namespace gm {

    export struct Glyph {
        u16 x;
        u16 y;
        u16 width;
        i16 advance;
        i16 left;
    };

    export enum class FontError {
        failed_to_open_file   = -1,
        invalid_header        = -2,
        data_corrupted        = -3,
        failed_to_load_sprite = -4
    };

    export class Font {
        static void _deleter(usize id) noexcept {
            static env::Function sprite_delete{ env::FunctionId::sprite_delete };
            sprite_delete(id);
        }

        using Sprite = Handle<usize, _deleter, -1>;

        std::string _name;
        Sprite _sprite;
        u16 _height;
        i16 _top;
        std::unordered_map<u32, Glyph> _glyphs;

    public:
        Font() noexcept = default;

        Font(std::string_view name, std::string_view sprite_path)
            : _name{ name } {

            static env::Function sprite_add{ env::FunctionId::sprite_add };
            _sprite.reset(static_cast<usize>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            if (!_sprite) {
                throw FontError::failed_to_load_sprite;
            }

            std::ifstream file{ std::filesystem::path{ sprite_path }.replace_extension("gly"), std::ios::binary };
            if (!file.is_open()) {
                throw FontError::failed_to_open_file;
            }

            auto read{ [&](auto& dest) noexcept {
                return static_cast<bool>(file.read(reinterpret_cast<char*>(&dest), sizeof(dest)));
            } };

            static constexpr std::array GLYPH_SIGN{ 'G', 'L', 'Y', '\1', '\1', '\0' };
            std::array<char, GLYPH_SIGN.size()> sign;
            if (!read(sign) || sign != GLYPH_SIGN) {
                throw FontError::invalid_header;
            }

            u32 size;
            if (!read(_height) || !read(_top) || !read(size)) {
                throw FontError::data_corrupted;
            }

            for (usize i{}; i != size; ++i) {
                u32 ch;
                Glyph glyph;
                if (!read(ch) || !read(glyph) || !_glyphs.try_emplace(ch, std::move(glyph)).second) {
                    throw FontError::data_corrupted;
                }
            }

            if (file.peek() != std::char_traits<char>::eof()) {
                throw FontError::data_corrupted;
            }
        }

        operator bool() const noexcept {
            return static_cast<bool>(_sprite);
        }

        std::string_view name() const noexcept {
            assert(*this);
            return _name;
        }

        usize sprite() const noexcept {
            assert(*this);
            return _sprite.get();
        }

        u16 height() const noexcept {
            assert(*this);
            return _height;
        }

        i16 top() const noexcept {
            assert(*this);
            return _top;
        }

        const auto& glyphs() const noexcept {
            assert(*this);
            return _glyphs;
        }
    };

}
