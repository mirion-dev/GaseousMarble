module;

#include <cassert>

export module gm:font;

import std;
import :core;
import :engine;

namespace gm {

    export class Font {
    public:
        struct Glyph {
            u16 x, y;
            u16 width;
            i16 advance;
            i16 left;
        };

        enum class Error {
            failed_to_open_file   = -1,
            invalid_header        = -2,
            data_corrupted        = -3,
            failed_to_load_sprite = -4
        };

    private:
        static void _deleter(usize id) noexcept {
            static Function sprite_delete{ Function::Id::sprite_delete };
            sprite_delete(id);
        }

        std::string _name;
        Handle<usize, _deleter, -1> _sprite;
        u16 _height;
        i16 _top;
        std::unordered_map<u32, Glyph> _glyphs;

    public:
        Font() noexcept = default;

        Font(std::string_view name, std::string_view sprite_path) :
            _name{ name } {

            static Function sprite_add{ Function::Id::sprite_add };
            _sprite.reset(static_cast<usize>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            if (!_sprite) {
                throw Error::failed_to_load_sprite;
            }

            std::u8string u8(sprite_path.size(), '\0');
            std::memcpy(u8.data(), sprite_path.data(), sprite_path.size());
            std::ifstream file{ std::filesystem::path{ u8 }.replace_extension(u8"gly"), std::ios::binary };
            if (!file.is_open()) {
                throw Error::failed_to_open_file;
            }

            auto read{
                [&](auto& dest) noexcept {
                    return static_cast<bool>(file.read(reinterpret_cast<char*>(&dest), sizeof(dest)));
                }
            };

            static constexpr std::array GLYPH_SIGN{ 'G', 'L', 'Y', '\1', '\1', '\0' };
            std::array<char, GLYPH_SIGN.size()> sign;
            if (!read(sign) || sign != GLYPH_SIGN) {
                throw Error::invalid_header;
            }

            u32 size;
            if (!read(_height) || !read(_top) || !read(size)) {
                throw Error::data_corrupted;
            }

            _glyphs.reserve(size);
            for (usize i{}; i != size; ++i) {
                u32 ch;
                Glyph glyph;
                if (!read(ch) || !read(glyph) || !_glyphs.emplace(ch, glyph).second) {
                    throw Error::data_corrupted;
                }
            }
            if (file.peek() != std::char_traits<char>::eof()) {
                throw Error::data_corrupted;
            }
        }

        bool empty() const noexcept {
            return !_sprite;
        }

        u16 height() const noexcept {
            assert(!empty());
            return _height;
        }

        i16 top() const noexcept {
            assert(!empty());
            return _top;
        }

        std::string_view name() const noexcept {
            assert(!empty());
            return _name;
        }

        usize sprite() const noexcept {
            assert(!empty());
            return _sprite.get();
        }

        const auto& glyphs() const noexcept {
            assert(!empty());
            return _glyphs;
        }
    };

}
