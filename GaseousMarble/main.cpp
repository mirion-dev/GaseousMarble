#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include <dwrite_3.h>
#include <wil/result.h>

import std;
import gm;

using namespace gm;

static std::unordered_map<std::string, Font> font_map;
static Draw draw;

const char* string_from_real(f64 real) noexcept {
    return reinterpret_cast<const char*>(static_cast<usize>(real));
}

API f64 gm_internal_string_to_real(const char* string) noexcept {
    return reinterpret_cast<usize>(string);
}

API f64 gm_internal_new_font(
    f64 key_real,
    f64 name_real,
    f64 size,
    f64 weight,
    f64 style,
    f64 stretch,
    f64 locale_real
) noexcept
try {
    std::string_view key{ string_from_real(key_real) };
    std::string_view name{ string_from_real(name_real) };
    std::string_view locale{ string_from_real(locale_real) };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    return font_map.try_emplace(
            std::string{ key },
            to_wstring(name),
            static_cast<f32>(size),
            static_cast<DWRITE_FONT_WEIGHT>(weight),
            static_cast<DWRITE_FONT_STYLE>(style),
            static_cast<DWRITE_FONT_STRETCH>(stretch),
            to_wstring(locale)
        ).second ?
        S_OK :
        S_FALSE;
}
CATCH_RETURN()

API f64 gm_delete_font(const char* font_key) noexcept {
    auto iter{ font_map.find(font_key) };
    if (iter == font_map.end()) {
        return S_FALSE;
    }

    if (draw.font() == &*iter) {
        draw.set_font(nullptr);
    }

    font_map.erase(iter);
    return S_OK;
}

API f64 gm_draw(f64 x, f64 y, const char* text) noexcept
try {
    draw.text(static_cast<f32>(x), static_cast<f32>(y), text);
    return S_OK;
}
CATCH_RETURN()

API f64 gm_set_font(const char* font_key) noexcept
try {
    auto iter{ font_map.find(font_key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API f64 gm_set_max_width(f64 max_width) noexcept {
    if (max_width <= 0 || max_width > std::numeric_limits<f32>::max()) {
        max_width = std::numeric_limits<f32>::max();
    }

    draw.set_max_width(static_cast<f32>(max_width));
    return S_OK;
}

API f64 gm_set_max_height(f64 max_height) noexcept {
    if (max_height <= 0 || max_height > std::numeric_limits<f32>::max()) {
        max_height = std::numeric_limits<f32>::max();
    }

    draw.set_max_height(static_cast<f32>(max_height));
    return S_OK;
}

API const char* gm_get_font() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? "" : font->first.data();
}

API f64 gm_get_max_width() noexcept {
    return draw.max_width();
}

API f64 gm_get_max_height() noexcept {
    return draw.max_height();
}
