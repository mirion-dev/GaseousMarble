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

        std::list<std::pair<K, V>> _data;
        std::unordered_map<
            std::conditional_t<sizeof(K) <= sizeof(void*) * 2, K, KRef>,
            typename decltype(_data)::iterator,
            Hash,
            std::equal_to<>
        > _map;

    public:
        template <class Key, class Fn>
        std::pair<std::pair<K, V>&, bool> get(Key&& key, Fn&& func) {
            auto map_iter{ _map.find(key) };
            if (map_iter != _map.end()) {
                auto iter{ map_iter->second };
                _data.splice(_data.end(), _data, iter);
                return { *iter, false };
            }

            if (_data.size() == N) {
                _map.erase(_data.front().first);
                _data.pop_front();
            }

            auto iter{ _data.emplace(_data.end(), std::forward<Key>(key), std::forward<Fn>(func)()) };
            _map.emplace(iter->first, iter);
            return { *iter, true };
        }

        void clear() noexcept {
            _map.clear();
            _data.clear();
        }
    };

}
