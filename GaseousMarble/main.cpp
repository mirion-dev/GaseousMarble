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
    f64 size_real,
    f64 weight_real,
    f64 style_real,
    f64 stretch_real,
    f64 locale_real
) noexcept
try {
    std::string key{ string_from_real(key_real) };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    return font_map.try_emplace(
            std::move(key),
            to_wstring(string_from_real(name_real)),
            static_cast<f32>(size_real),
            static_cast<DWRITE_FONT_WEIGHT>(weight_real),
            static_cast<DWRITE_FONT_STYLE>(style_real),
            static_cast<DWRITE_FONT_STRETCH>(stretch_real),
            to_wstring(string_from_real(locale_real))
        ).second
        ? S_OK
        : S_FALSE;
}
CATCH_RETURN()

API f64 gm_delete_font(const char* key) noexcept {
    auto iter{ font_map.find(key) };
    if (iter == font_map.end()) {
        return S_FALSE;
    }

    if (draw.font() == &*iter) {
        draw.set_font(nullptr);
    }

    font_map.erase(iter);
    return S_OK;
}

API f64 gm_draw_text(f64 x_real, f64 y_real, const char* text) noexcept
try {
    draw.text(static_cast<f32>(x_real), static_cast<f32>(y_real), text);
    return S_OK;
}
CATCH_RETURN()

API f64 gm_set_alignment(f64 alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(static_cast<u8>(alignment_real) & DrawOption::alignment_mask) };
    if ((alignment & DrawOption::alignment_mask_v) == DrawOption::alignment_invalid) {
        alignment = alignment & ~DrawOption::alignment_mask_v | DrawOption::alignment_horizon;
    }

    draw.set_alignment(alignment);
    return S_OK;
}

API f64 gm_set_alignment_h(f64 alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(
        draw.alignment() & ~DrawOption::alignment_mask_h
        | static_cast<u8>(alignment_real) & DrawOption::alignment_mask_h
    ) };
    draw.set_alignment(alignment);
    return S_OK;
}

API f64 gm_set_alignment_v(f64 alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(
        draw.alignment() & ~DrawOption::alignment_mask_v
        | static_cast<u8>(alignment_real) & DrawOption::alignment_mask_v
    ) };
    if ((alignment & DrawOption::alignment_mask_v) == DrawOption::alignment_invalid) {
        alignment = alignment & ~DrawOption::alignment_mask_v | DrawOption::alignment_horizon;
    }

    draw.set_alignment(alignment);
    return S_OK;
}

API f64 gm_set_max_width(f64 max_width_real) noexcept {
    if (max_width_real <= 0 || max_width_real > std::numeric_limits<f32>::max()) {
        max_width_real = std::numeric_limits<f32>::max();
    }

    draw.set_max_width(static_cast<f32>(max_width_real));
    return S_OK;
}

API f64 gm_set_max_height(f64 max_height_real) noexcept {
    if (max_height_real <= 0 || max_height_real > std::numeric_limits<f32>::max()) {
        max_height_real = std::numeric_limits<f32>::max();
    }

    draw.set_max_height(static_cast<f32>(max_height_real));
    return S_OK;
}

API f64 gm_set_font(const char* key) noexcept
try {
    auto iter{ font_map.find(key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API f64 gm_get_alignment() noexcept {
    return draw.alignment();
}

API f64 gm_get_alignment_h() noexcept {
    return draw.alignment() & DrawOption::alignment_mask_h;
}

API f64 gm_get_alignment_v() noexcept {
    return draw.alignment() & DrawOption::alignment_mask_v;
}

API f64 gm_get_max_width() noexcept {
    return draw.max_width();
}

API f64 gm_get_max_height() noexcept {
    return draw.max_height();
}

API const char* gm_get_font() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? "" : font->first.data();
}
