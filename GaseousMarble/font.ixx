module;

#include <cassert>

export module gm.font;

import std;
import gm.types;
import gm.utils;
import gm.env;

namespace gm {

    export enum class FontError {
        failed_to_open_file   = -1,
        invalid_header        = -2,
        data_corrupted        = -3,
        failed_to_load_sprite = -4
    };

}

export template <>
struct std::is_error_code_enum<gm::FontError> : std::true_type {};

namespace gm {

    class FontErrorCategory : public std::error_category {
    public:
        const char* name() const noexcept {
            return "gm.font";
        }

        std::string message(int value) const {
            switch (static_cast<FontError>(value)) {
            case FontError::failed_to_open_file:
                return "Failed to open glyph data file.";
            case FontError::invalid_header:
                return "Invalid glyph data header.";
            case FontError::data_corrupted:
                return "Glyph data is corrupted.";
            case FontError::failed_to_load_sprite:
                return "Failed to load font sprite.";
            }
            return "Unknown font error.";
        }
    };

    export const std::error_category& font_error_category() noexcept {
        static FontErrorCategory category;
        return category;
    }

    export std::error_code make_error_code(FontError error) noexcept {
        return { static_cast<int>(error), font_error_category() };
    }

    export struct Glyph {
        u16 x;
        u16 y;
        u16 width;
        i16 advance;
        i16 left;
    };

    export class Font {
        static inline usize _id_counter{ 1 };

        static void _sprite_deleter(usize id) noexcept {
            static env::Function sprite_delete{ env::FunctionId::sprite_delete };
            sprite_delete(id);
        }

        usize _id{ _id_counter++ };
        Handle<usize, _sprite_deleter, -1> _sprite;
        u16 _glyph_height;
        i16 _glyph_top;
        std::unordered_map<u32, Glyph> _glyphs;

    public:
        Font() noexcept = default;

        explicit Font(std::string_view sprite_path) {
            static env::Function sprite_add{ env::FunctionId::sprite_add };
            _sprite.reset(static_cast<isize>(sprite_add(sprite_path, 1, false, false, 0, 0)));
            if (!_sprite) {
                throw std::system_error{ FontError::failed_to_load_sprite };
            }

            std::ifstream file{ std::filesystem::path{ sprite_path }.replace_extension("gly"), std::ios::binary };
            if (!file.is_open()) {
                throw std::system_error{ FontError::failed_to_open_file };
            }

            auto read{ [&](auto& dest) noexcept {
                return static_cast<bool>(file.read(reinterpret_cast<char*>(&dest), sizeof(dest)));
            } };

            static constexpr std::array GLYPH_SIGN{ 'G', 'L', 'Y', '\1', '\1', '\0' };
            std::array<char, GLYPH_SIGN.size()> sign;
            if (!read(sign) || sign != GLYPH_SIGN) {
                throw std::system_error{ FontError::invalid_header };
            }

            u32 size;
            if (!read(_glyph_height) || !read(_glyph_top) || !read(size)) {
                throw std::system_error{ FontError::data_corrupted };
            }

            for (usize i{}; i != size; ++i) {
                u32 ch;
                Glyph glyph;
                if (!read(ch) || !read(glyph) || !_glyphs.try_emplace(ch, std::move(glyph)).second) {
                    throw std::system_error{ FontError::data_corrupted };
                }
            }

            if (file.peek() != std::char_traits<char>::eof()) {
                throw std::system_error{ FontError::data_corrupted };
            }
        }

        operator bool() const noexcept {
            return static_cast<bool>(_sprite);
        }

        usize id() const noexcept {
            return _id;
        }

        usize sprite() const noexcept {
            assert(*this);
            return _sprite.get();
        }

        usize glyph_height() const noexcept {
            assert(*this);
            return _glyph_height;
        }

        isize glyph_top() const noexcept {
            assert(*this);
            return _glyph_top;
        }

        const auto& glyphs() const noexcept {
            assert(*this);
            return _glyphs;
        }
    };

}
