module;

#include <cassert>

export module gm.font;

import std;
import gm.types;
import gm.utils;
import gm.engine;

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

            Error error;
            auto throw_if_failed{
                [&](bool expr) {
                    if (!expr) {
                        throw error;
                    }
                }
            };

            error = Error::failed_to_load_sprite;
            static Function sprite_add{ Function::Id::sprite_add };
            _sprite.reset(static_cast<usize>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            throw_if_failed(_sprite.is_valid());

            error = Error::failed_to_open_file;
            std::filesystem::path path{ std::u8string{ sprite_path.begin(), sprite_path.end() } };
            std::ifstream file{ path.replace_extension(u8"gly"), std::ios::binary };
            throw_if_failed(file.is_open());

            auto read{
                [&](auto& dest) noexcept {
                    return static_cast<bool>(file.read(reinterpret_cast<char*>(&dest), sizeof(dest)));
                }
            };

            error = Error::invalid_header;
            static constexpr std::array GLYPH_SIGN{ 'G', 'L', 'Y', '\1', '\1', '\0' };
            std::array<char, GLYPH_SIGN.size()> sign;
            throw_if_failed(read(sign) && sign == GLYPH_SIGN);

            error = Error::data_corrupted;
            u32 size;
            throw_if_failed(read(_height) && read(_top) && read(size));

            _glyphs.reserve(size);
            for (usize i{}; i != size; ++i) {
                u32 ch;
                Glyph glyph;
                throw_if_failed(read(ch) && read(glyph) && _glyphs.emplace(ch, glyph).second);
            }

            throw_if_failed(file.peek() == std::char_traits<char>::eof());
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
