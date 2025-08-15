module;

#include <icu.h>

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

    // ---------------
    // text processing
    // ---------------

    export std::optional<std::u16string> to_utf16(std::string_view utf8) noexcept {
        i32 size;
        UErrorCode status{};
        u_strFromUTF8(nullptr, 0, &size, utf8.data(), utf8.size(), &status);
        if (status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(status)) {
            return {};
        }

        std::u16string utf16(size, '\0');
        status = U_ZERO_ERROR;
        u_strFromUTF8(utf16.data(), size, nullptr, utf8.data(), utf8.size(), &status);
        if (U_FAILURE(status)) {
            return {};
        }

        return utf16;
    }

    export struct Token {
        std::u16string_view text;
        UWordBreak type;
    };

    export std::optional<std::vector<Token>> tokenize(std::u16string_view text) noexcept {
        UErrorCode status{};

        std::unique_ptr<UBreakIterator, decltype(&ubrk_close)> iter{
            ubrk_open(UBRK_WORD, nullptr, text.data(), text.size(), &status),
            ubrk_close
        };
        if (U_FAILURE(status)) {
            return {};
        }

        std::vector<Token> tokens;
        i32 first{ ubrk_first(iter.get()) };
        while (true) {
            i32 last{ ubrk_next(iter.get()) };
            if (last == -1) {
                break;
            }

            tokens.emplace_back(
                std::u16string_view{ text.data() + first, text.data() + last },
                static_cast<UWordBreak>(ubrk_getRuleStatus(iter.get()))
            );

            first = last;
        }
        return tokens;
    }

}
