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
            saturating_cast<u16>(raw_properties, Font::PROPERTIES_NORMAL),
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

    if (draw.option().font == &*iter) {
        draw.option().font = nullptr;
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
    draw.option().alignment = saturating_cast<u8>(raw_alignment, 0) & DrawOption::ALIGNMENT_MASK;
    return S_OK;
}

API Real gm2_set_text_alignment(Real raw_alignment) noexcept {
    draw.option().alignment = draw.option().alignment & ~DrawOption::TEXT_ALIGNMENT_MASK
        | saturating_cast<u8>(raw_alignment, 0) & DrawOption::TEXT_ALIGNMENT_MASK;
    return S_OK;
}

API Real gm2_set_par_alignment(Real raw_alignment) noexcept {
    draw.option().alignment = draw.option().alignment & ~DrawOption::PAR_ALIGNMENT_MASK
        | saturating_cast<u8>(raw_alignment, 0) & DrawOption::PAR_ALIGNMENT_MASK;
    return S_OK;
}

API Real gm2_set_word_wrapping(Real raw_word_wrapping) noexcept {
    int word_wrapping{ saturating_cast<u8>(raw_word_wrapping, DWRITE_WORD_WRAPPING_WHOLE_WORD) };
    if (word_wrapping == DWRITE_WORD_WRAPPING_WRAP) {
        word_wrapping = DWRITE_WORD_WRAPPING_WHOLE_WORD;
    }

    draw.option().word_wrapping = static_cast<DWRITE_WORD_WRAPPING>(word_wrapping);
    return S_OK;
}

API Real gm2_set_text_direction(Real raw_direction) noexcept {
    int direction{ saturating_cast<u8>(raw_direction, 0) };
    if (direction == DWRITE_READING_DIRECTION_TOP_TO_BOTTOM) {
        direction = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
    }
    else if (direction == DWRITE_READING_DIRECTION_BOTTOM_TO_TOP) {
        direction = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
    }

    draw.option().direction = draw.option().direction & ~DrawOption::TEXT_DIRECTION_MASK
        | direction & DrawOption::TEXT_DIRECTION_MASK;
    return S_OK;
}

API Real gm2_set_par_direction(Real raw_direction) noexcept {
    int direction{ saturating_cast<u8>(raw_direction, 0) };
    if (direction == DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT) {
        direction = DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM;
    }
    else if (direction == DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT) {
        direction = DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP;
    }

    draw.option().direction = draw.option().direction & ~DrawOption::PAR_DIRECTION_MASK
        | direction & DrawOption::PAR_DIRECTION_MASK;
    return S_OK;
}

API Real gm2_set_direction(Real raw_direction) noexcept {
    gm2_set_text_direction(raw_direction);
    gm2_set_par_direction(raw_direction);
    return S_OK;
}

API Real gm2_set_tab_spacing(Real raw_tab_spacing) noexcept {
    draw.option().tab_spacing = saturating_cast<f32>(raw_tab_spacing, 0, std::numeric_limits<f32>::max());
    return S_OK;
}

API Real gm2_set_trimming(Real raw_trimming) noexcept {
    draw.option().trimming = static_cast<DWRITE_TRIMMING_GRANULARITY>(saturating_cast<u8>(raw_trimming, 0));
    return S_OK;
}

API Real gm2_set_max_width(Real raw_max_width) noexcept {
    f32 max_width{ saturating_cast<f32>(raw_max_width) };
    if (max_width < 0) {
        max_width = std::numeric_limits<f32>::max();
    }

    draw.option().max_width = max_width;
    return S_OK;
}

API Real gm2_set_max_height(Real raw_max_height) noexcept {
    f32 max_height{ saturating_cast<f32>(raw_max_height) };
    if (max_height < 0) {
        max_height = std::numeric_limits<f32>::max();
    }

    draw.option().max_height = max_height;
    return S_OK;
}

API Real gm2_set_font(StringRef raw_key) noexcept
try {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.option().font = &*iter;
    return S_OK;
}
CATCH_RETURN()

API Real gm2_set_pair_kerning(Real raw_pair_kerning) noexcept {
    draw.option().pair_kerning = static_cast<bool>(raw_pair_kerning);
    return S_OK;
}

API Real gm2_set_letter_spacing(Real raw_letter_spacing) noexcept {
    draw.option().letter_spacing = saturating_cast<f32>(raw_letter_spacing);
    return S_OK;
}

API Real gm2_set_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.option().line_spacing_type = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
    draw.option().line_height = saturating_cast<f32>(raw_line_height);
    draw.option().baseline = saturating_cast<f32>(raw_baseline);
    return S_OK;
}

API Real gm2_set_fixed_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.option().line_spacing_type = DWRITE_LINE_SPACING_METHOD_UNIFORM;
    draw.option().line_height = saturating_cast<f32>(raw_line_height);
    draw.option().baseline = saturating_cast<f32>(raw_baseline);
    return S_OK;
}

API Real gm2_set_line_height(Real raw_line_height) noexcept {
    draw.option().line_height = saturating_cast<f32>(raw_line_height, 0, std::numeric_limits<f32>::max());
    return S_OK;
}

API Real gm2_set_baseline(Real raw_baseline) noexcept {
    draw.option().baseline = saturating_cast<f32>(raw_baseline);
    return S_OK;
}

API Real gm2_get_text_alignment() noexcept {
    return draw.option().alignment & DrawOption::TEXT_ALIGNMENT_MASK;
}

API Real gm2_get_par_alignment() noexcept {
    return draw.option().alignment & DrawOption::PAR_ALIGNMENT_MASK;
}

API Real gm2_get_word_wrapping() noexcept {
    return draw.option().word_wrapping;
}

API Real gm2_get_text_direction() noexcept {
    return draw.option().direction & DrawOption::TEXT_DIRECTION_MASK;
}

API Real gm2_get_par_direction() noexcept {
    return draw.option().direction & DrawOption::PAR_DIRECTION_MASK;
}

API Real gm2_get_tab_spacing() noexcept {
    return draw.option().tab_spacing;
}

API Real gm2_get_trimming() noexcept {
    return draw.option().trimming;
}

API Real gm2_get_max_width() noexcept {
    return draw.option().max_width;
}

API Real gm2_get_max_height() noexcept {
    return draw.option().max_height;
}

API StringRef gm2_get_font() noexcept {
    string_value = draw.option().font == nullptr ? String{} : String{ draw.option().font->first };
    return string_value.data();
}

API StringRef gm2_get_font_name() noexcept {
    string_value = draw.option().font == nullptr ? String{} : String{ to_string(draw.option().font->second.name()) };
    return string_value.data();
}

API Real gm2_get_font_size() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.size();
}

API Real gm2_get_font_weight() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.weight();
}

API Real gm2_get_font_style() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.style();
}

API Real gm2_get_font_stretch() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.stretch();
}

API StringRef gm2_get_font_locale() noexcept {
    string_value = draw.option().font == nullptr ? String{} : String{ to_string(draw.option().font->second.locale()) };
    return string_value.data();
}

API Real gm2_get_font_min_aa_h_size() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.min_aa_h_size();
}

API Real gm2_get_font_min_aa_v_size() noexcept {
    return draw.option().font == nullptr ? -1 : draw.option().font->second.min_aa_v_size();
}

API Real gm2_get_pair_kerning() noexcept {
    return draw.option().pair_kerning;
}

API Real gm2_get_letter_spacing() noexcept {
    return draw.option().letter_spacing;
}

API Real gm2_get_line_spacing_type() noexcept {
    return draw.option().line_spacing_type;
}

API Real gm2_get_line_height() noexcept {
    return draw.option().line_height;
}

API Real gm2_get_baseline() noexcept {
    return draw.option().baseline;
}
