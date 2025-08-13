export module gm:core;

import std;

namespace gm {

    // -----------------
    // fundamental types
    // -----------------

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

    }

    // -----------------
    // utility functions
    // -----------------

    export std::generator<u32> utf8_decode(std::string_view str) noexcept {
        static constexpr u32 REP_CHAR{ 0xfffd };

        auto i{ reinterpret_cast<const u8*>(str.data()) };
        auto end{ i + str.size() };
        while (i != end) {
            if (*i >> 7 == 0) {
                co_yield *i++;
            }
            else if (*i >> 5 == 0x06) {
                if (end - i < 1) {
                    co_yield REP_CHAR;
                    break;
                }

                auto ch{ static_cast<u32>((*i & 0x1f) << 6 | i[1] & 0x3f) };
                if (i[1] >> 6 != 0x02 || ch <= 0x7f) {
                    co_yield REP_CHAR;
                }
                else {
                    co_yield ch;
                }
                i += 2;
            }
            else if (*i >> 4 == 0x0e) {
                if (end - i < 2) {
                    co_yield REP_CHAR;
                    break;
                }

                auto ch{ static_cast<u32>((*i & 0x0f) << 12 | (i[1] & 0x3f) << 6 | i[2] & 0x3f) };
                if (i[1] >> 6 != 0x02 || i[2] >> 6 != 0x02 || ch <= 0x7ff || ch >= 0xd800 && ch <= 0xdfff) {
                    co_yield REP_CHAR;
                }
                else {
                    co_yield ch;
                }
                i += 3;
            }
            else if (*i >> 3 == 0x1e) {
                if (end - i < 3) {
                    co_yield REP_CHAR;
                    break;
                }

                auto ch{ static_cast<u32>((*i & 0x07) << 18 | (i[1] & 0x3f) << 12 | (i[2] & 0x3f) << 6 | i[3] & 0x3f) };
                if (i[1] >> 6 != 0x02 || i[2] >> 6 != 0x02 || i[3] >> 6 != 0x02 || ch <= 0xffff || ch >= 0x110000) {
                    co_yield REP_CHAR;
                }
                else {
                    co_yield ch;
                }
                i += 4;
            }
            else {
                co_yield REP_CHAR;
                ++i;
            }
        }
    }

}
