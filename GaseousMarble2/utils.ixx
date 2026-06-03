export module gm.utils;

import std;
import gm.types;

namespace gm {

    export std::wstring to_wstring(std::string_view str) noexcept {
        std::u8string str_u8{ str.begin(), str.end() };
        return std::filesystem::path{ str_u8 }.wstring();
    }

    export template <class K, class V, usize N>
        requires (N > 0)
    class Cache {
        struct KRef {
            const K* ptr;

            KRef(const K& key) noexcept :
                ptr{ &key } {}

            template <class T>
            friend bool operator==(KRef left, const T& right) noexcept {
                if constexpr (std::same_as<T, KRef>) {
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
                if constexpr (std::same_as<T, KRef>) {
                    return std::hash<K>{}(*value.ptr);
                }
                else {
                    return std::hash<T>{}(value);
                }
            }
        };

        std::unordered_map<K, V, Hash, std::equal_to<>> _data;
        std::deque<std::conditional_t<sizeof(K) <= sizeof(void*) * 2, K, KRef>> _order;

    public:
        Cache() noexcept = default;
        Cache(const Cache&) noexcept = delete;
        Cache(Cache&&) noexcept = default;

        Cache& operator=(const Cache&) noexcept = delete;
        Cache& operator=(Cache&&) noexcept = default;

        template <class Key, class Fn>
        std::pair<std::pair<const K, V>&, bool> get(Key&& key, Fn&& func) {
            auto iter{ _data.find(std::forward<Key>(key)) };
            if (iter != _data.end()) {
                return { *iter, false };
            }

            iter = _data.emplace(std::forward<Key>(key), std::forward<Fn>(func)()).first;
            _order.push_back(iter->first);

            if (_data.size() > N) {
                _data.erase(_order.front());
                _order.pop_front();
            }

            return { *iter, true };
        }

        void clear() noexcept {
            _data.clear();
            _order.clear();
        }
    };

}
