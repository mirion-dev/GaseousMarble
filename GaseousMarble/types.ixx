export module gm.types;

import std;

namespace gm {

    export {
        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;

        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;

        using f32 = float;
        using f64 = double;

        using isize = std::ptrdiff_t;
        using usize = std::size_t;
    }

    // See https://docwiki.embarcadero.com/RADStudio/Athens/en/Unicode_in_RAD_Studio#New_String_Type:_UnicodeString
    export template <class C>
    class BasicString {
        struct Header {
            u16 code_page{ sizeof(C) == 1 ? 65001 : sizeof(C) == 2 ? 1200 : 12000 };
            u16 char_size{ sizeof(C) };
            u32 ref_count{ 1 };
            u32 size{};
        };

        struct Empty {
            C* data;

            Empty() noexcept {
                // WORKAROUND: Prevent `_empty` from being placed in .rdata.
                auto storage{ new u8[sizeof(Header) + sizeof(C)] };
                *reinterpret_cast<Header*>(storage) = {};
                data = reinterpret_cast<C*>(storage + sizeof(Header));
                *data = {};
            }
        };

        static inline Empty _empty;

        C* _data{ _empty.data };

        auto _header() noexcept {
            return std::launder(reinterpret_cast<Header*>(reinterpret_cast<u8*>(_data) - sizeof(Header)));
        }

        auto _header() const noexcept {
            return std::launder(reinterpret_cast<const Header*>(reinterpret_cast<const u8*>(_data) - sizeof(Header)));
        }

    public:
        BasicString() noexcept {
            ++_header()->ref_count;
        }

        template <std::convertible_to<std::basic_string_view<C>> V>
        explicit BasicString(const V& str) noexcept {
            auto view{ static_cast<std::basic_string_view<C>>(str) };
            auto storage{ new u8[sizeof(Header) + (view.size() + 1) * sizeof(C)] };
            *reinterpret_cast<Header*>(storage) = { .size = view.size() };
            _data = reinterpret_cast<C*>(storage + sizeof(Header));
            *std::ranges::copy(view, _data).out = {};
        }

        BasicString(const BasicString& other) noexcept :
            _data{ other._data } {

            ++_header()->ref_count;
        }

        BasicString(BasicString&& other) noexcept :
            BasicString{} {

            this->swap(other);
        }

        ~BasicString() noexcept {
            if (--_header()->ref_count == 0) {
                delete[](reinterpret_cast<u8*>(_data) - sizeof(Header));
            }
        }

        BasicString& operator=(const BasicString& other) noexcept {
            BasicString temp{ other };
            this->swap(temp);
            return *this;
        }

        BasicString& operator=(BasicString&& other) noexcept {
            this->swap(other);
            return *this;
        }

        operator std::basic_string_view<C>() const noexcept {
            return { _data, size() };
        }

        void swap(BasicString& other) noexcept {
            std::ranges::swap(_data, other._data);
        }

        friend void swap(BasicString& left, BasicString& right) noexcept {
            left.swap(right);
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
        using StringRef = const char*;
    }

}
