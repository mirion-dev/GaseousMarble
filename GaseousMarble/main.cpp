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

API Real gm2_internal_to_real(String string) noexcept {
    return reinterpret_cast<usize>(string);
}

String internal_from_real(Real real) noexcept {
    return reinterpret_cast<const char*>(static_cast<usize>(real));
}

API Real gm2_internal_new_font(
    Real key_real,
    Real name_real,
    Real size_real,
    Real properties_real,
    Real locale_real,
    Real min_antialiasing_h_size_real,
    Real min_antialiasing_v_size_real
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
            saturating_cast<u32>(properties_real),
            to_wstring(internal_from_real(locale_real)),
            saturating_cast<f32>(min_antialiasing_h_size_real),
            saturating_cast<f32>(min_antialiasing_v_size_real)
        ).second
        ? S_OK
        : S_FALSE;
}
CATCH_RETURN()

API Real gm2_delete_font(String key) noexcept {
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

API Real gm2_draw_text(Real x_real, Real y_real, String text) noexcept
try {
    draw.text(saturating_cast<f32>(x_real), saturating_cast<f32>(y_real), text);
    return S_OK;
}
CATCH_RETURN()

API Real gm2_text_width(String text) noexcept
try {
    return draw.text_width(text);
}
CATCH_RETURN()

API Real gm2_text_height(String text) noexcept
try {
    return draw.text_height(text);
}
CATCH_RETURN()

API Real gm2_set_alignment(Real alignment_real) noexcept {
    draw.set_alignment(static_cast<u8>(saturating_cast<u8>(alignment_real) & DrawOption::ALIGNMENT_MASK));
    return S_OK;
}

API Real gm2_set_alignment_h(Real alignment_real) noexcept {
    draw.set_alignment(
        static_cast<u8>(
            draw.alignment() & ~DrawOption::ALIGNMENT_H_MASK
            | saturating_cast<u8>(alignment_real) & DrawOption::ALIGNMENT_H_MASK
        )
    );
    return S_OK;
}

API Real gm2_set_alignment_v(Real alignment_real) noexcept {
    draw.set_alignment(
        static_cast<u8>(
            draw.alignment() & ~DrawOption::ALIGNMENT_V_MASK
            | saturating_cast<u8>(alignment_real) & DrawOption::ALIGNMENT_V_MASK
        )
    );
    return S_OK;
}

API Real gm2_set_max_width(Real max_width_real) noexcept {
    f32 max_width{ saturating_cast<f32>(max_width_real) };
    if (max_width <= 0) {
        max_width = std::numeric_limits<f32>::max();
    }

    draw.set_max_width(max_width);
    return S_OK;
}

API Real gm2_set_max_height(Real max_height_real) noexcept {
    f32 max_height{ saturating_cast<f32>(max_height_real) };
    if (max_height <= 0) {
        max_height = std::numeric_limits<f32>::max();
    }

    draw.set_max_height(max_height);
    return S_OK;
}

API Real gm2_set_font(String key) noexcept
try {
    auto iter{ font_map.find(key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API Real gm2_set_line_spacing(Real line_height_real, Real baseline_real) noexcept {
    draw.set_fixed_line_spacing(false);
    draw.set_line_height(saturating_cast<f32>(line_height_real));
    draw.set_baseline(saturating_cast<f32>(baseline_real));
    return S_OK;
}

API Real gm2_set_fixed_line_spacing(Real line_height_real, Real baseline_real) noexcept {
    draw.set_fixed_line_spacing(true);
    draw.set_line_height(saturating_cast<f32>(line_height_real));
    draw.set_baseline(saturating_cast<f32>(baseline_real));
    return S_OK;
}

API Real gm2_set_line_height(Real line_height_real) noexcept {
    draw.set_line_height(saturating_cast<f32>(line_height_real));
    return S_OK;
}

API Real gm2_set_baseline(Real baseline_real) noexcept {
    draw.set_baseline(saturating_cast<f32>(baseline_real));
    return S_OK;
}

API Real gm2_get_alignment() noexcept {
    return draw.alignment();
}

API Real gm2_get_alignment_h() noexcept {
    return draw.alignment() & DrawOption::ALIGNMENT_H_MASK;
}

API Real gm2_get_alignment_v() noexcept {
    return draw.alignment() & DrawOption::ALIGNMENT_V_MASK;
}

API Real gm2_get_max_width() noexcept {
    return draw.max_width();
}

API Real gm2_get_max_height() noexcept {
    return draw.max_height();
}

API String gm2_get_font() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? "" : font->first.data();
}

API Real gm2_is_fixed_line_spacing() noexcept {
    return draw.is_fixed_line_spacing();
}

API Real gm2_get_line_height() noexcept {
    return draw.line_height();
}

API Real gm2_get_baseline() noexcept {
    return draw.baseline();
}
