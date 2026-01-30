module;

#include <icu.h>
#include <wil/resource.h>

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

        using f32 = float;
        using f64 = double;

        using isize = std::ptrdiff_t;
        using usize = std::size_t;

    }

#pragma endregion

#pragma region utilities

    export template <class T, auto Deleter, T Null = {}>
    using Handle = wil::unique_any<T, decltype(Deleter), Deleter, wil::details::pointer_access_all, T, T, Null>;

    export struct Hash {
        using is_transparent = int;

        template <class T>
        usize operator()(const T& value) const noexcept {
            return std::hash<T>{}(value);
        }
    };

    export template <class K, class V, usize N>
        requires (N > 0)
    class Cache {
        struct Ref {
            const K* ptr;

            Ref(const K& key) noexcept :
                ptr{ &key } {}

            template <class T>
            friend bool operator==(Ref left, const T& right) noexcept {
                if constexpr (std::same_as<T, Ref>) {
                    return *left.ptr == *right.ptr;
                }
                else {
                    return *left.ptr == right;
                }
            }
        };

        struct Hash {
            using is_transparent = int;

            template <class T>
            usize operator()(const T& value) const noexcept {
                if constexpr (std::same_as<T, Ref>) {
                    return std::hash<K>{}(*value.ptr);
                }
                else {
                    return std::hash<T>{}(value);
                }
            }
        };

        std::list<std::pair<K, V>> _list;
        std::unordered_map<Ref, typename decltype(_list)::iterator, Hash, std::equal_to<>> _map;

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

#pragma region text handling

    export bool is_white_space(u32 ch) noexcept {
        return u_isUWhiteSpace(ch);
    }

    export bool is_line_break(u32 ch) noexcept {
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

    export bool is_wide(u32 ch) noexcept {
        switch (u_getIntPropertyValue(ch, UCHAR_EAST_ASIAN_WIDTH)) {
        case U_EA_FULLWIDTH:
        case U_EA_WIDE:
            return true;
        default:
            return false;
        }
    }

    bool unicode_for_each(std::string_view str, const auto& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, str.data(), str.size(), &error) };
        if (!iter) {
            return false;
        }

        while (true) {
            i32 ch{ utext_next32(iter.get()) };
            if (ch == -1 || !func(static_cast<u32>(ch))) {
                return true;
            }
        }
    }

    export bool word_break_for_each(std::string_view str, const auto& func) noexcept {
        UErrorCode error{};
        Handle<UText*, utext_close> iter{ utext_openUTF8(nullptr, str.data(), str.size(), &error) };
        if (!iter) {
            return false;
        }

        Handle<UBreakIterator*, ubrk_close> breaker{ ubrk_open(UBRK_WORD, nullptr, nullptr, 0, &error) };
        if (!breaker) {
            return false;
        }

        ubrk_setUText(breaker.get(), iter.get(), &error);
        if (error > 0) {
            return false;
        }

        const char* ptr{ str.data() };
        isize first{};
        while (true) {
            isize last{ ubrk_next(breaker.get()) };
            if (last == -1 || !func(std::string_view{ ptr + first, ptr + last }, ubrk_getRuleStatus(breaker.get()))) {
                return true;
            }
            first = last;
        }
    }

#pragma endregion

}
