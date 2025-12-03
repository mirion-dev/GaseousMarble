module;

#include <icu.h>

#define STD_BEGIN } namespace std {
#define STD_END } namespace gm {

export module gm:core;

import std;

namespace gm {

#pragma region fundamental types

    export {

        using i8 = std::int8_t;
        using i16 = std::int16_t;
        using i32 = std::int32_t;
        using i64 = std::int64_t;

        using u8 = std::uint8_t;
        using u16 = std::uint16_t;
        using u32 = std::uint32_t;
        using u64 = std::uint64_t;

        using c8 = char8_t;
        using c16 = char16_t;
        using c32 = char32_t;

        using f32 = float;
        using f64 = double;

        using isize = std::ptrdiff_t;
        using usize = std::size_t;

    }

#pragma endregion

#pragma region text handling

    export bool is_white_space(c32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

    export bool is_line_break(c32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_LINE_BREAK)) {
        case U_LB_MANDATORY_BREAK:
        case U_LB_CARRIAGE_RETURN:
        case U_LB_LINE_FEED:
        case U_LB_NEXT_LINE:
            return true;
        default:
            return false;
        }
    }

    export bool is_wide(c32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH)) {
        case U_EA_FULLWIDTH:
        case U_EA_WIDE:
            return true;
        default:
            return false;
        }
    }

    export bool unicode_for_each(std::u8string_view str, auto func) noexcept {
        const c8* ptr{ str.data() };
        usize size{ str.size() };
        for (usize i{}; i != size;) {
            i32 ch;
            U8_NEXT(ptr, i, size, ch);
            if (ch < 0 || !func(static_cast<c32>(ch))) {
                return false;
            }
        }
        return true;
    }

    export bool unicode_for_each(std::u16string_view str, auto func) noexcept {
        const c16* ptr{ str.data() };
        usize size{ str.size() };
        for (usize i{}; i != size;) {
            i32 ch;
            U16_NEXT(ptr, i, size, ch);
            if (ch < 0 || !func(static_cast<c32>(ch))) {
                return false;
            }
        }
        return true;
    }

    export bool word_break_for_each(std::u16string_view str, auto func) noexcept {
        const c16* ptr{ str.data() };
        usize size{ str.size() };
        UErrorCode error{};
        std::unique_ptr<UBreakIterator, decltype(&ubrk_close)> iter{
            ubrk_open(UBRK_WORD, nullptr, ptr, size, &error),
            ubrk_close
        };
        if (U_FAILURE(error)) {
            return false;
        }

        for (i32 first{ ubrk_first(iter.get()) }, last; ; first = last) {
            last = ubrk_next(iter.get());
            if (last == UBRK_DONE) {
                break;
            }

            if (!func(std::u16string_view{ ptr + first, ptr + last }, ubrk_getRuleStatus(iter.get()))) {
                return false;
            }
        }
        return true;
    }

#pragma endregion

#pragma region utilities

    export struct Hash {
        using is_transparent = int;

        template <class T>
        usize operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
        }
    };

    template <class K>
    struct Wrapper {
        const K* ptr;

        Wrapper(const K& key) noexcept :
            ptr{ &key } {}

        friend bool operator==(Wrapper left, Wrapper right) noexcept {
            return *left.ptr == *right.ptr;
        }
    };

STD_BEGIN

    template <class K>
    struct hash<gm::Wrapper<K>> {
        gm::usize operator()(gm::Wrapper<K> wrapper) const noexcept {
            return hash<K>{}(*wrapper.ptr);
        }
    };

STD_END

    export template <class K, class V, usize N>
        requires (N > 0)
    class Cache {
        // workaround for DevCom-10969873
        static constexpr std::hash<Wrapper<K>> DUMMY;

        std::list<std::pair<K, V>> _list;
        std::unordered_map<Wrapper<K>, typename decltype(_list)::iterator> _map;

    public:
        using iterator = decltype(_list)::iterator;

        Cache() noexcept = default;

        template <class Key, class... Args>
        std::pair<iterator, bool> try_emplace(Key&& key, Args&&... args) {
            auto map_iter{ _map.find(key) };
            if (map_iter != _map.end()) {
                iterator iter{ map_iter->second };
                _list.splice(_list.end(), _list, iter);
                return { iter, false };
            }

            if (_list.size() == N) {
                _map.erase(_list.front().first);
                _list.pop_front();
            }

            iterator iter{ _list.emplace(_list.end(), std::forward<Key>(key), V(std::forward<Args>(args)...)) };
            _map.emplace(iter->first, iter);
            return { iter, true };
        }

        void clear() noexcept {
            _map.clear();
            _list.clear();
        }
    };

#pragma endregion

}
