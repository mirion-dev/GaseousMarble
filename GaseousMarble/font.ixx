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

        static constexpr auto ID_NULL{ static_cast<usize>(-1) };

    private:
        struct Handle {
            usize id{ ID_NULL };

            Handle() noexcept = default;

            Handle(std::nullptr_t) noexcept {};

            Handle(usize id) noexcept :
                id{ id } {}

            operator bool() const noexcept {
                return id != ID_NULL;
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

        Sprite(std::u8string_view path) {
            static Function sprite_add{ Function::Id::sprite_add };
            usize id{ static_cast<usize>(static_cast<isize>(sprite_add(path, 1, false, false, 0, 0))) };
            if (id == ID_NULL) {
                throw Error::failed_to_add_sprite;
            }

            _ptr.reset(id);
        }

        bool empty() const noexcept {
            return _ptr == nullptr;
        }

        usize id() const noexcept {
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
            no_error              = 0,
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

            static constexpr char GLYPH_SIGN[]{ 'G', 'L', 'Y', 1, 0, 0 };
            char sign[sizeof(GLYPH_SIGN)];
            if (!file.read(sign, sizeof(sign)) || !std::ranges::equal(sign, GLYPH_SIGN)) {
                throw Error::invalid_header;
            }

            if (!file.read(reinterpret_cast<char*>(&_height), sizeof(_height))
                || !file.read(reinterpret_cast<char*>(&_top), sizeof(_top))) {
                throw Error::data_corrupted;
            }

            c32 ch;
            Glyph glyph;
            while (file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
                if (!file.read(reinterpret_cast<char*>(&glyph), sizeof(glyph))) {
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
