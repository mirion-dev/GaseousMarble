export module gm.core;

import std;

// fundamental types
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

// utility functions
export namespace gm::core {

    // not fully conforming to RFC 3629
    std::generator<u32> utf8_decode(std::string_view str) noexcept {
        auto i{ reinterpret_cast<const u8*>(str.data()) }, end{ i + str.size() };
        for (; i != end; ++i) {
            u32 ch{ 0xfffd };

            if (*i >> 7 == 0) {
                ch = *i;
            }
            else if (*i >> 5 == 0x06) {
                if (i[1] >> 6 == 0x02) {
                    ch = (*i & 0x1f) << 6 | i[1] & 0x3f;
                    i += 1;
                }
            }
            else if (*i >> 4 == 0x0e) {
                if (i[1] >> 6 == 0x02 && i[2] >> 6 == 0x02) {
                    ch = (*i & 0x0f) << 12 | (i[1] & 0x3f) << 6 | i[2] & 0x3f;
                    i += 2;
                }
            }
            else if (*i >> 3 == 0x1e) {
                if (i[1] >> 6 == 0x02 && i[2] >> 6 == 0x02 && i[3] >> 6 == 0x02) {
                    ch = (*i & 0x07) << 18 | (i[1] & 0x3f) << 12 | (i[2] & 0x3f) << 6 | i[3] & 0x3f;
                    i += 3;
                }
            }

            co_yield ch;
        }
    }

}