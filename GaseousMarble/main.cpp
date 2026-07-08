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

static FontManager font_manager;
static std::unordered_map<std::string, usize> key_map;
static std::unordered_map<usize, std::string_view> id_map;
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

    if (key_map.contains(key)) {
        return S_FALSE;
    }

    usize id{ font_manager.insert(
        to_wstring(internal_from_real(raw_name)),
        saturating_cast<f32>(raw_size),
        saturating_cast<u16>(raw_properties, Font::PROPERTIES_NORMAL),
        to_wstring(internal_from_real(raw_locale)),
        std::max(saturating_cast<f32>(raw_min_aa_h_size), 0.f),
        std::max(saturating_cast<f32>(raw_min_aa_v_size), 0.f)
    ).first };
    id_map.try_emplace(id, key_map.try_emplace(std::move(key), id).first->first);
    return S_OK;
}
CATCH_RETURN()

API Real gm2_delete_font(StringRef raw_key) noexcept {
    auto iter{ key_map.find(raw_key) };
    if (iter == key_map.end()) {
        return S_FALSE;
    }

    usize id{ iter->second };
    if (draw.option().font.second == id) {
        draw.option().font = {};
    }

    font_manager.erase(id);
    key_map.erase(iter);
    id_map.erase(id);
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
    int direction{ saturating_cast<u8>(raw_direction, 0) & DrawOption::TEXT_DIRECTION_MASK };
    if (direction == DWRITE_READING_DIRECTION_TOP_TO_BOTTOM) {
        direction = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
    }
    else if (direction == DWRITE_READING_DIRECTION_BOTTOM_TO_TOP) {
        direction = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
    }

    draw.option().direction = draw.option().direction & ~DrawOption::TEXT_DIRECTION_MASK | direction;
    return S_OK;
}

API Real gm2_set_par_direction(Real raw_direction) noexcept {
    int direction{ saturating_cast<u8>(raw_direction, 0) & DrawOption::PAR_DIRECTION_MASK };
    if (direction == DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT) {
        direction = DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM;
    }
    else if (direction == DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT) {
        direction = DWRITE_FLOW_DIRECTION_BOTTOM_TO_TOP;
    }

    draw.option().direction = draw.option().direction & ~DrawOption::PAR_DIRECTION_MASK | direction;
    return S_OK;
}

API Real gm2_set_direction(Real raw_direction) noexcept {
    gm2_set_text_direction(raw_direction);
    gm2_set_par_direction(raw_direction);
    return S_OK;
}

API Real gm2_set_tab_spacing(Real raw_tab_spacing) noexcept {
    draw.option().tab_spacing = std::max(saturating_cast<f32>(raw_tab_spacing), 0.f);
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
    auto iter{ key_map.find(raw_key) };
    if (iter == key_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.option().font = { &font_manager, iter->second };
    return S_OK;
}
CATCH_RETURN()

API Real gm2_set_letter_spacing(Real raw_letter_spacing) noexcept {
    draw.option().letter_spacing = saturating_cast<f32>(raw_letter_spacing);
    return S_OK;
}

API Real gm2_set_line_height(Real raw_line_height) noexcept {
    draw.option().line_height = std::max(saturating_cast<f32>(raw_line_height), 0.f);
    return S_OK;
}

API Real gm2_set_baseline(Real raw_baseline) noexcept {
    draw.option().baseline = saturating_cast<f32>(raw_baseline);
    return S_OK;
}

API Real gm2_set_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.option().line_spacing_type = DWRITE_LINE_SPACING_METHOD_PROPORTIONAL;
    gm2_set_line_height(raw_line_height);
    gm2_set_baseline(raw_baseline);
    return S_OK;
}

API Real gm2_set_fixed_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.option().line_spacing_type = DWRITE_LINE_SPACING_METHOD_UNIFORM;
    gm2_set_line_height(raw_line_height);
    gm2_set_baseline(raw_baseline);
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
    usize id{ draw.option().font.second };
    string_value = id == 0 ? String{} : String{ id_map.at(id) };
    return string_value.data();
}

API StringRef gm2_get_font_name() noexcept {
    usize id{ draw.option().font.second };
    string_value = id == 0 ? String{} : String{ to_string(font_manager.get(id)->name()) };
    return string_value.data();
}

API Real gm2_get_font_size() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->size();
}

API Real gm2_get_font_weight() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->properties() & Font::WEIGHT_MASK;
}

API Real gm2_get_font_style() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->properties() & Font::STYLE_MASK;
}

API Real gm2_get_font_stretch() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->properties() & Font::STRETCH_MASK;
}

API StringRef gm2_get_font_locale() noexcept {
    usize id{ draw.option().font.second };
    string_value = id == 0 ? String{} : String{ to_string(font_manager.get(id)->locale()) };
    return string_value.data();
}

API Real gm2_get_font_min_aa_h_size() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->min_aa_h_size();
}

API Real gm2_get_font_min_aa_v_size() noexcept {
    usize id{ draw.option().font.second };
    return id == 0 ? -1 : font_manager.get(id)->min_aa_v_size();
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
