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

using Real = f64;
using String = const char*;

API Real gm_internal_to_real(String string) noexcept {
    return reinterpret_cast<usize>(string);
}

String internal_from_real(Real real) noexcept {
    return reinterpret_cast<const char*>(static_cast<usize>(real));
}

API Real gm_internal_new_font(
    Real key_real,
    Real name_real,
    Real size_real,
    Real weight_real,
    Real style_real,
    Real stretch_real,
    Real locale_real
) noexcept
try {
    std::string key{ internal_from_real(key_real) };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    return font_map.try_emplace(
            std::move(key),
            to_wstring(internal_from_real(name_real)),
            saturating_cast<f32>(size_real),
            static_cast<DWRITE_FONT_WEIGHT>(saturating_cast<int>(weight_real)),
            static_cast<DWRITE_FONT_STYLE>(saturating_cast<int>(style_real)),
            static_cast<DWRITE_FONT_STRETCH>(saturating_cast<int>(stretch_real)),
            to_wstring(internal_from_real(locale_real))
        ).second
        ? S_OK
        : S_FALSE;
}
CATCH_RETURN()

API Real gm_delete_font(String key) noexcept {
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

API Real gm_draw_text(Real x_real, Real y_real, String text) noexcept
try {
    draw.text(saturating_cast<f32>(x_real), saturating_cast<f32>(y_real), text);
    return S_OK;
}
CATCH_RETURN()

API Real gm_text_width(String text) noexcept
try {
    return draw.text_width(text);
}
CATCH_RETURN()

API Real gm_text_height(String text) noexcept
try {
    return draw.text_height(text);
}
CATCH_RETURN()

API Real gm_set_alignment(Real alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(saturating_cast<u8>(alignment_real) & DrawOption::alignment_mask) };
    if ((alignment & DrawOption::alignment_mask_v) == DrawOption::alignment_invalid) {
        alignment = alignment & ~DrawOption::alignment_mask_v | DrawOption::alignment_horizon;
    }

    draw.set_alignment(alignment);
    return S_OK;
}

API Real gm_set_alignment_h(Real alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(
        draw.alignment() & ~DrawOption::alignment_mask_h
        | saturating_cast<u8>(alignment_real) & DrawOption::alignment_mask_h
    ) };
    draw.set_alignment(alignment);
    return S_OK;
}

API Real gm_set_alignment_v(Real alignment_real) noexcept {
    u8 alignment{ static_cast<u8>(
        draw.alignment() & ~DrawOption::alignment_mask_v
        | saturating_cast<u8>(alignment_real) & DrawOption::alignment_mask_v
    ) };
    if ((alignment & DrawOption::alignment_mask_v) == DrawOption::alignment_invalid) {
        alignment = alignment & ~DrawOption::alignment_mask_v | DrawOption::alignment_horizon;
    }

    draw.set_alignment(alignment);
    return S_OK;
}

API Real gm_set_max_width(Real max_width_real) noexcept {
    f32 max_width{ saturating_cast<f32>(max_width_real) };
    if (max_width <= 0) {
        max_width = std::numeric_limits<f32>::max();
    }

    draw.set_max_width(max_width);
    return S_OK;
}

API Real gm_set_max_height(Real max_height_real) noexcept {
    f32 max_height{ saturating_cast<f32>(max_height_real) };
    if (max_height <= 0) {
        max_height = std::numeric_limits<f32>::max();
    }

    draw.set_max_height(max_height);
    return S_OK;
}

API Real gm_set_font(String key) noexcept
try {
    auto iter{ font_map.find(key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API Real gm_get_alignment() noexcept {
    return draw.alignment();
}

API Real gm_get_alignment_h() noexcept {
    return draw.alignment() & DrawOption::alignment_mask_h;
}

API Real gm_get_alignment_v() noexcept {
    return draw.alignment() & DrawOption::alignment_mask_v;
}

API Real gm_get_max_width() noexcept {
    return draw.max_width();
}

API Real gm_get_max_height() noexcept {
    return draw.max_height();
}

API String gm_get_font() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? "" : font->first.data();
}
