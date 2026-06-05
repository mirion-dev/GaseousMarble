module;

#include <d3d8.h>
#undef interface

export module gm.engine;

import std;
import gm.types;

namespace gm {

    // https://docwiki.embarcadero.com/RADStudio/Athens/en/Unicode_in_RAD_Studio#New_String_Type:_UnicodeString

    struct StringHeader {
        u16 code_page;
        u16 char_size;
        u32 ref_count;
        u32 size;
    };

    template <class C>
    static constexpr u16 CODE_PAGE{ sizeof(C) == 1 ? 65001 : sizeof(C) == 2 ? 1200 : 12000 };

    template <class C>
    class EmptyString {
        alignas(StringHeader) u8 _storage[sizeof(StringHeader) + sizeof(C)];
        C* _data;

    public:
        EmptyString() noexcept {
            *reinterpret_cast<StringHeader*>(_storage) = { CODE_PAGE<C>, sizeof(C), 1, 0 };
            _data = reinterpret_cast<C*>(_storage + sizeof(StringHeader));
            *_data = {};
        }

        C* data() noexcept {
            return _data;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    template <class C>
    static EmptyString<C> empty_string;

    export template <class C>
    class BasicStringView {
        const C* _data{ empty_string<C>.data() };

        auto _header() const noexcept {
            return std::launder(
                reinterpret_cast<const StringHeader*>(reinterpret_cast<const u8*>(_data) - sizeof(StringHeader))
            );
        }

    public:
        BasicStringView() noexcept = default;

        BasicStringView(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept :
            _data{ static_cast<std::basic_string_view<C>>(str).data() } {}

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        usize size() const noexcept {
            return _header()->size;
        }

        usize ref_count() const noexcept {
            return _header()->ref_count;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    export template <class C>
    class BasicString {
        C* _data{ empty_string<C>.data() };

        auto _header() noexcept {
            return std::launder(reinterpret_cast<StringHeader*>(reinterpret_cast<u8*>(_data) - sizeof(StringHeader)));
        }

        auto _header() const noexcept {
            return std::launder(
                reinterpret_cast<const StringHeader*>(reinterpret_cast<const u8*>(_data) - sizeof(StringHeader))
            );
        }

    public:
        BasicString() noexcept {
            ++_header()->ref_count;
        }

        BasicString(const std::convertible_to<std::basic_string_view<C>> auto& str) noexcept {
            auto view{ static_cast<std::basic_string_view<C>>(str) };
            auto storage{ new u8[sizeof(StringHeader) + (view.size() + 1) * sizeof(C)] };
            *reinterpret_cast<StringHeader*>(storage) = { CODE_PAGE<C>, sizeof(C), 1, view.size() };
            _data = reinterpret_cast<C*>(storage + sizeof(StringHeader));
            *std::ranges::copy(view, _data).out = {};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        BasicString(BasicString&& other) noexcept {
            swap(other);
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](reinterpret_cast<u8*>(_data) - sizeof(StringHeader));
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            swap(temp);
            return *this;
        }

        BasicString& operator=(BasicString&& other) noexcept {
            swap(other);
            return *this;
        }

        void swap(BasicString& other) noexcept {
            std::ranges::swap(_data, other._data);
        }

        friend void swap(BasicString& left, BasicString& right) noexcept {
            left.swap(right);
        }

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        bool empty() const noexcept {
            return size() == 0;
        }

        usize size() const noexcept {
            return _header()->size;
        }

        usize ref_count() const noexcept {
            return _header()->ref_count;
        }

        const C* data() const noexcept {
            return _data;
        }
    };

    export {

        using Real = f64;

        using String = BasicString<char>;
        using StringView = BasicStringView<char>;

    }

    export class Direct3D {
        struct Resource {
            IDirect3D8* interface;
            IDirect3DDevice8* device;
            u64 _;
            u32 render_width;
            u32 render_height;
        };

        static inline const auto _resource_ptr{ reinterpret_cast<Resource*>(0x006886a4) };

    public:
        static IDirect3D8* interface() noexcept {
            return _resource_ptr->interface;
        }

        static IDirect3DDevice8* device() noexcept {
            return _resource_ptr->device;
        }

        static usize render_width() noexcept {
            return _resource_ptr->render_width;
        }

        static usize render_height() noexcept {
            return _resource_ptr->render_height;
        }
    };

}
