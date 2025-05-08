module;

#include <cassert>
#include <d3dx8.h>

#undef interface

export module gm:engine;

import std;
import :core;

// fundamental types of GML
namespace gm {

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        u32 ref_count;
        u32 size;
    };

    export template <class Ch>
    class BasicStringView {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(Ch) };

        const Ch* _data;

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        // `str` must point to a Delphi UnicodeString structure or accessing its header is undefined
        BasicStringView(std::basic_string_view<Ch> str = {}) noexcept :
            _data{ str.data() } {}

        operator std::basic_string_view<Ch>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const Ch* data() const noexcept {
            return _data;
        }
    };

    export template <class Ch>
    class BasicString {
        static constexpr u32 _offset{ sizeof(StringHeader) / sizeof(Ch) };
        static constexpr u16 _cp_utf8{ 65001 };
        static constexpr u16 _cp_utf16{ 1200 };
        static constexpr u16 _cp_utf32{ 12000 };

        Ch* _data;

        auto _header() noexcept {
            return reinterpret_cast<StringHeader*>(_data - _offset);
        }

        auto _header() const noexcept {
            return reinterpret_cast<const StringHeader*>(_data - _offset);
        }

    public:
        BasicString() noexcept {
            static BasicString empty_str{ "" };
            _data = empty_str._data;

            ++_header()->ref_count;
        }

        BasicString(std::basic_string_view<Ch> str) noexcept :
            _data{ new Ch[_offset + str.size() + 1] + _offset } {

            new(_header()) StringHeader{
                sizeof(Ch) == 1 ? _cp_utf8 : sizeof(Ch) == 2 ? _cp_utf16 : _cp_utf32,
                sizeof(Ch),
                1,
                str.size()
            };
            new(std::uninitialized_copy(str.begin(), str.end(), _data)) Ch{};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](_data - _offset);
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            std::swap(_data, temp._data);
            return *this;
        }

        operator BasicStringView<Ch>() const noexcept {
            return _data;
        }

        operator std::basic_string_view<Ch>() const noexcept {
            return { _data, _header()->size };
        }

        u32 size() const noexcept {
            return _header()->size;
        }

        u32 ref_count() const noexcept {
            return _header()->ref_count;
        }

        const Ch* data() const noexcept {
            return _data;
        }
    };

    export {
        using Real = f64;

        using String = BasicString<char>;
        using String16 = BasicString<char16_t>;
        using String32 = BasicString<char32_t>;

        using StringView = BasicStringView<char>;
        using String16View = BasicStringView<char16_t>;
        using String32View = BasicStringView<char32_t>;
    }

}

// interface of GameMaker Direct3D resources
namespace gm {

    struct Direct3DResource {
        IDirect3D8* interface;
        IDirect3DDevice8* device;
        u64 _;
        u32 render_width;
        u32 render_height;
    };

    export class IDirect3DResource {
        static constexpr auto _resource{ reinterpret_cast<Direct3DResource*>(0x006886a4) };

    public:
        static IDirect3D8* interface() noexcept {
            return _resource->interface;
        }

        static IDirect3DDevice8* device() noexcept {
            return _resource->device;
        }

        static u32 render_width() noexcept {
            return _resource->render_width;
        }

        static u32 render_height() noexcept {
            return _resource->render_height;
        }
    };

}
