module;

#include <cassert>

export module gm:font;

import std;
import :core;
import :engine;

namespace gm {

    export class Sprite {
    public:
        enum class Error {
            failed_to_add_sprite = -1
        };

    private:
        struct Deleter {
            using pointer = usize;

            void operator()(pointer id) const noexcept {
                static Function sprite_delete{ Function::Id::sprite_delete };
                sprite_delete(id);
            }
        };

        std::unique_ptr<usize, Deleter> _ptr;

    public:
        Sprite() noexcept = default;

        Sprite(std::u8string_view path) {
            static Function sprite_add{ Function::Id::sprite_add };
            usize id{ static_cast<usize>(static_cast<isize>(sprite_add(path, 1, false, false, 0, 0))) + 1 };
            if (id == 0) {
                throw Error::failed_to_add_sprite;
            }

            _ptr.reset(id);
        }

        bool empty() const noexcept {
            return _ptr == nullptr;
        }

        usize id() const noexcept {
            return _ptr.get() - 1;
        }
    };

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
        u16 _height;
        i16 _top;
        std::u8string _name;
        Sprite _sprite;
        std::unordered_map<c32, Glyph> _glyphs;

    public:
        Font() noexcept = default;

        Font(std::u8string_view name, std::u8string_view sprite_path)
        try:
            _name{ name },
            _sprite{ sprite_path } {

            std::ifstream file{ std::filesystem::path{ sprite_path }.replace_extension("gly"), std::ios::binary };
            if (!file.is_open()) {
                throw Error::failed_to_open_file;
            }

            auto read{
                [&](auto& dest) noexcept {
                    return static_cast<bool>(file.read(reinterpret_cast<char*>(&dest), sizeof(dest)));
                }
            };

            static constexpr std::array GLYPH_SIGN{ 'G', 'L', 'Y', '\1', '\0', '\0' };
            std::array<char, GLYPH_SIGN.size()> sign;
            if (!read(sign) || sign != GLYPH_SIGN) {
                throw Error::invalid_header;
            }

            if (!read(_height) || !read(_top)) {
                throw Error::data_corrupted;
            }

            c32 ch;
            Glyph glyph;
            while (read(ch)) {
                if (!read(glyph)) {
                    throw Error::data_corrupted;
                }
                _glyphs.emplace(ch, glyph);
            }
            if (!file.eof()) {
                throw Error::data_corrupted;
            }
        }
        catch (Sprite::Error) {
            throw Error::failed_to_load_sprite;
        }

        bool empty() const noexcept {
            return _sprite.empty();
        }

        u16 height() const noexcept {
            assert(!empty());
            return _height;
        }

        i16 top() const noexcept {
            assert(!empty());
            return _top;
        }

        std::u8string_view name() const noexcept {
            assert(!empty());
            return _name;
        }

        const Sprite& sprite() const noexcept {
            assert(!empty());
            return _sprite;
        }

        const auto& glyphs() const noexcept {
            assert(!empty());
            return _glyphs;
        }
    };

}
