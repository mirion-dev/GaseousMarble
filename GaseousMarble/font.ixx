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
            no_error             = 0,
            failed_to_add_sprite = -1
        };

        static constexpr i32 NULL_ID{ -1 };

    private:
        struct Handle {
            i32 id{ NULL_ID };

            Handle() noexcept = default;

            Handle(std::nullptr_t) noexcept {};

            Handle(i32 id) noexcept :
                id{ id } {}

            operator bool() const noexcept {
                return id != NULL_ID;
            }

            bool operator==(Handle other) const noexcept {
                return id == other.id;
            }
        };

        struct Deleter {
            using pointer = Handle;

            void operator()(pointer handle) const noexcept {
                static Function sprite_delete{ Function::Id::sprite_delete };
                sprite_delete(handle.id);
            }
        };

        std::unique_ptr<Handle, Deleter> _ptr;

    public:
        Sprite() noexcept = default;

        Sprite(std::string_view path) {
            static Function sprite_add{ Function::Id::sprite_add };
            auto id{ static_cast<i32>(sprite_add(path, 1, false, false, 0, 0)) };
            if (id == NULL_ID) {
                throw Error::failed_to_add_sprite;
            }

            _ptr.reset(id);
        }

        i32 id() const noexcept {
            return _ptr.get().id;
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
            no_error             = 0,
            failed_to_open_file  = -1,
            invalid_header       = -2,
            data_corrupted       = -3,
            failed_to_add_sprite = -4
        };

    private:
        u16 _height;
        i16 _top;
        std::string _name;
        Sprite _sprite;
        std::unordered_map<i32, Glyph> _glyphs;

    public:
        Font() noexcept = default;

        Font(std::string_view font_name, std::string_view sprite_path)
        try:
            _name{ font_name },
            _sprite{ sprite_path } {

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
                file.read(reinterpret_cast<char*>(&_glyphs[ch]), sizeof(_glyphs[ch]));
            }
            if (!file.eof()) {
                throw Error::data_corrupted;
            }
        }
        catch (Sprite::Error) {
            throw Error::failed_to_add_sprite;
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

        const Sprite& sprite() const noexcept {
            assert(_sprite != nullptr);
            return _sprite;
        }

        const auto& glyphs() const noexcept {
            assert(_sprite != nullptr);
            return _glyphs;
        }
    };

}
