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

static String string_value;

static std::unordered_map<std::string, Font> font_map;
static Draw draw;

API Real gm2_internal_to_real(StringRef string) noexcept {
    return reinterpret_cast<usize>(string);
}

StringRef internal_from_real(Real real) noexcept {
    return reinterpret_cast<const char*>(static_cast<usize>(real));
}

API Real gm2_internal_new_font(
    Real raw_key,
    Real raw_name,
    Real raw_size,
    Real raw_properties,
    Real raw_locale,
    Real raw_min_aa_h_size,
    Real raw_min_aa_v_size
) noexcept
try {
    std::string key{ internal_from_real(raw_key) };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    return font_map.try_emplace(
            std::move(key),
            to_wstring(internal_from_real(raw_name)),
            saturating_cast<f32>(raw_size),
            saturating_cast<u32>(raw_properties),
            to_wstring(internal_from_real(raw_locale)),
            saturating_cast<f32>(raw_min_aa_h_size),
            saturating_cast<f32>(raw_min_aa_v_size)
        ).second
        ? S_OK
        : S_FALSE;
}
CATCH_RETURN()

API Real gm2_delete_font(StringRef raw_key) noexcept {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        return S_FALSE;
    }

    if (draw.font() == &*iter) {
        draw.set_font(nullptr);
    }

    font_map.erase(iter);
    return S_OK;
}

API Real gm2_draw_text(Real raw_x, Real raw_y, StringRef raw_text) noexcept
try {
    draw.text(saturating_cast<f32>(raw_x), saturating_cast<f32>(raw_y), to_wstring(raw_text));
    return S_OK;
}
CATCH_RETURN()

API Real gm2_text_width(StringRef raw_text) noexcept
try {
    return draw.text_width(to_wstring(raw_text));
}
CATCH_RETURN()

API Real gm2_text_height(StringRef raw_text) noexcept
try {
    return draw.text_height(to_wstring(raw_text));
}
CATCH_RETURN()

API Real gm2_set_alignment(Real raw_alignment) noexcept {
    draw.set_alignment(saturating_cast<u8>(raw_alignment) & DrawOption::ALIGNMENT_MASK);
    return S_OK;
}

API Real gm2_set_alignment_h(Real raw_alignment) noexcept {
    draw.set_alignment(
        draw.alignment() & ~DrawOption::ALIGNMENT_H_MASK
        | saturating_cast<u8>(raw_alignment) & DrawOption::ALIGNMENT_H_MASK
    );
    return S_OK;
}

API Real gm2_set_alignment_v(Real raw_alignment) noexcept {
    draw.set_alignment(
        draw.alignment() & ~DrawOption::ALIGNMENT_V_MASK
        | saturating_cast<u8>(raw_alignment) & DrawOption::ALIGNMENT_V_MASK
    );
    return S_OK;
}

API Real gm2_set_direction(Real raw_direction) noexcept {
    draw.set_direction(saturating_cast<u8>(raw_direction) & DrawOption::DIRECTION_MASK);
    return S_OK;
}

API Real gm2_set_direction_h(Real raw_direction) noexcept {
    draw.set_direction(
        draw.direction() & ~DrawOption::DIRECTION_H_MASK
        | saturating_cast<u8>(raw_direction) & DrawOption::DIRECTION_H_MASK
    );
    return S_OK;
}

API Real gm2_set_direction_v(Real raw_direction) noexcept {
    draw.set_direction(
        draw.direction() & ~DrawOption::DIRECTION_V_MASK
        | saturating_cast<u8>(raw_direction) & DrawOption::DIRECTION_V_MASK
    );
    return S_OK;
}

API Real gm2_set_max_width(Real raw_max_width) noexcept {
    f32 max_width{ saturating_cast<f32>(raw_max_width) };
    if (max_width <= 0) {
        max_width = std::numeric_limits<f32>::max();
    }

    draw.set_max_width(max_width);
    return S_OK;
}

API Real gm2_set_max_height(Real raw_max_height) noexcept {
    f32 max_height{ saturating_cast<f32>(raw_max_height) };
    if (max_height <= 0) {
        max_height = std::numeric_limits<f32>::max();
    }

    draw.set_max_height(max_height);
    return S_OK;
}

API Real gm2_set_font(StringRef raw_key) noexcept
try {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.set_font(&*iter);
    return S_OK;
}
CATCH_RETURN()

API Real gm2_set_pair_kerning(Real raw_use_pair_kerning) noexcept {
    draw.set_pair_kerning(saturating_cast<bool>(raw_use_pair_kerning));
    return S_OK;
}

API Real gm2_set_letter_spacing(Real raw_letter_spacing) noexcept {
    draw.set_letter_spacing(saturating_cast<f32>(raw_letter_spacing));
    return S_OK;
}

API Real gm2_set_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.set_fixed_line_spacing(false);
    draw.set_line_height(saturating_cast<f32>(raw_line_height));
    draw.set_baseline(saturating_cast<f32>(raw_baseline));
    return S_OK;
}

API Real gm2_set_fixed_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.set_fixed_line_spacing(true);
    draw.set_line_height(saturating_cast<f32>(raw_line_height));
    draw.set_baseline(saturating_cast<f32>(raw_baseline));
    return S_OK;
}

API Real gm2_set_line_height(Real raw_line_height) noexcept {
    draw.set_line_height(saturating_cast<f32>(raw_line_height));
    return S_OK;
}

API Real gm2_set_baseline(Real raw_baseline) noexcept {
    draw.set_baseline(saturating_cast<f32>(raw_baseline));
    return S_OK;
}

API Real gm2_get_alignment_h() noexcept {
    return draw.alignment() & DrawOption::ALIGNMENT_H_MASK;
}

API Real gm2_get_alignment_v() noexcept {
    return draw.alignment() & DrawOption::ALIGNMENT_V_MASK;
}

API Real gm2_get_direction_h() noexcept {
    return draw.direction() & DrawOption::DIRECTION_H_MASK;
}

API Real gm2_get_direction_v() noexcept {
    return draw.direction() & DrawOption::DIRECTION_V_MASK;
}

API Real gm2_get_max_width() noexcept {
    return draw.max_width();
}

API Real gm2_get_max_height() noexcept {
    return draw.max_height();
}

API StringRef gm2_get_font() noexcept {
    auto font{ draw.font() };
    string_value = font == nullptr ? String{} : String{ font->first };
    return string_value.data();
}

API StringRef gm2_get_font_name() noexcept {
    auto font{ draw.font() };
    string_value = font == nullptr ? String{} : String{ to_string(font->second.name()) };
    return string_value.data();
}

API Real gm2_get_font_size() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.size();
}

API Real gm2_get_font_weight() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.weight();
}

API Real gm2_get_font_style() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.style();
}

API Real gm2_get_font_stretch() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.stretch();
}

API StringRef gm2_get_font_locale() noexcept {
    auto font{ draw.font() };
    string_value = font == nullptr ? String{} : String{ to_string(font->second.locale()) };
    return string_value.data();
}

API Real gm2_get_font_min_aa_h_size() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.min_aa_h_size();
}

API Real gm2_get_font_min_aa_v_size() noexcept {
    auto font{ draw.font() };
    return font == nullptr ? -1 : font->second.min_aa_v_size();
}

API Real gm2_use_pair_kerning() noexcept {
    return draw.use_pair_kerning();
}

API Real gm2_get_letter_spacing() noexcept {
    return draw.letter_spacing();
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
