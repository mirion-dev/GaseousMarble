#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include "log.h"

#include <dwrite_3.h>
#include <eigen3/Eigen/Geometry>
#include <spdlog/spdlog.h>
#include <wil/result.h>

import std;
import gm;

using namespace gm;

String string_value;

std::unordered_map<std::string, DrawOption> option_map;
std::unordered_map<std::string, Font> font_map;
std::unordered_map<usize, std::string_view> id_map;
Draw draw{ env::config().layout_cache_size };

template <std::floating_point R>
R saturating_cast(Real value) noexcept {
    return gm::saturating_cast<R>(value, -LayoutOption::INFINITY_, LayoutOption::INFINITY_);
}

API Real gm2_internal_to_real(StringRef string) noexcept {
    return reinterpret_cast<usize>(string);
}

StringRef from_real(Real real) noexcept {
    return reinterpret_cast<const char*>(static_cast<usize>(real));
}

API Real
gm2_internal_new_font(Real raw_key, Real raw_name, Real raw_properties, Real raw_size, Real raw_locale) noexcept try {
    std::string key{ from_real(raw_key) };
    if (key.empty()) {
        throw std::invalid_argument{ "Font key must not be empty." };
    }

    if (font_map.contains(key)) { // Silent failure because duplicate keys may also occur after the game restart.
        return S_FALSE;
    }

    if (font_map.size() >= env::config().max_font_num) {
        throw std::runtime_error{ "Too many fonts." };
    }

    std::wstring name{ to_wstring(from_real(raw_name)) };
    u16 properties{ saturating_cast<u16>(raw_properties, FontDesc{}.properties) };
    f32 size{ saturating_cast<f32>(raw_size) };
    std::wstring locale{ to_wstring(from_real(raw_locale)) };
    if (locale.empty()) {
        locale = env::default_locale();
    }

    FontDesc desc{ std::filesystem::is_regular_file(name)
                       ? FontDesc::from(name, properties, size, locale)
                       : FontDesc{ {}, std::move(name), properties, size, std::move(locale) } };
    if (!desc.is_valid()) {
        throw std::invalid_argument{ "Invalid font description." };
    }

    GlyphAtlas atlas{ env::config().atlas, env::config().raster };
    auto iter{ font_map.try_emplace(std::move(key), std::move(desc), std::move(atlas)).first };
    id_map.try_emplace(iter->second.id(), iter->first);
    return S_OK;
}
GM_CATCH_RETURN()

API Real gm2_draw_text(Real raw_x, Real raw_y, StringRef raw_text) noexcept try {
    draw.text({ saturating_cast<f32>(raw_x), saturating_cast<f32>(raw_y) }, to_wstring(raw_text));
    return S_OK;
}
GM_CATCH_RETURN()

API Real gm2_save_options(StringRef raw_key) noexcept {
    option_map.insert_or_assign(raw_key, draw.option());
    return S_OK;
}

API Real gm2_load_options(StringRef raw_key) noexcept try {
    auto iter{ option_map.find(raw_key) };
    if (iter == option_map.end()) {
        throw std::runtime_error{ "Options not found." };
    }

    draw.option() = iter->second;
    return S_OK;
}
GM_CATCH_RETURN()

API Real gm2_set_alignment(Real raw_alignment) noexcept {
    draw.option().alignment = saturating_cast<u8>(raw_alignment, 0) & DrawOption::ALIGNMENT_MASK;
    return S_OK;
}

API Real gm2_set_text_alignment(Real raw_text_alignment) noexcept {
    draw.option().alignment = draw.option().alignment & ~DrawOption::TEXT_ALIGNMENT_MASK
                              | saturating_cast<u8>(raw_text_alignment, 0) & DrawOption::TEXT_ALIGNMENT_MASK;
    return S_OK;
}

API Real gm2_set_par_alignment(Real raw_par_alignment) noexcept {
    draw.option().alignment = draw.option().alignment & ~DrawOption::PAR_ALIGNMENT_MASK
                              | saturating_cast<u8>(raw_par_alignment, 0) & DrawOption::PAR_ALIGNMENT_MASK;
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

API Real gm2_set_text_direction(Real raw_text_direction) noexcept {
    int direction{ saturating_cast<u8>(raw_text_direction, 0) & DrawOption::TEXT_DIRECTION_MASK };
    if (direction == DWRITE_READING_DIRECTION_TOP_TO_BOTTOM) {
        direction = DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
    } else if (direction == DWRITE_READING_DIRECTION_BOTTOM_TO_TOP) {
        direction = DWRITE_READING_DIRECTION_RIGHT_TO_LEFT;
    }

    draw.option().direction = draw.option().direction & ~DrawOption::TEXT_DIRECTION_MASK | direction;
    return S_OK;
}

API Real gm2_set_par_direction(Real raw_par_direction) noexcept {
    int direction{ saturating_cast<u8>(raw_par_direction, 0) & DrawOption::PAR_DIRECTION_MASK };
    if (direction == DWRITE_FLOW_DIRECTION_LEFT_TO_RIGHT) {
        direction = DWRITE_FLOW_DIRECTION_TOP_TO_BOTTOM;
    } else if (direction == DWRITE_FLOW_DIRECTION_RIGHT_TO_LEFT) {
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
        max_width = LayoutOption::INFINITY_;
    }

    draw.option().max_size.x() = max_width;
    return S_OK;
}

API Real gm2_set_max_height(Real raw_max_height) noexcept {
    f32 max_height{ saturating_cast<f32>(raw_max_height) };
    if (max_height < 0) {
        max_height = LayoutOption::INFINITY_;
    }

    draw.option().max_size.y() = max_height;
    return S_OK;
}

API Real gm2_set_max_size(Real raw_max_width, Real raw_max_height) noexcept {
    gm2_set_max_width(raw_max_width);
    gm2_set_max_height(raw_max_height);
    return S_OK;
}

API Real gm2_set_font(StringRef raw_key) noexcept try {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        throw std::invalid_argument{ "Font not found." };
    }

    draw.option().font = { &iter->second, iter->second.id() };
    return S_OK;
}
GM_CATCH_RETURN()

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

API Real gm2_set_uniform_line_spacing(Real raw_line_height, Real raw_baseline) noexcept {
    draw.option().line_spacing_type = DWRITE_LINE_SPACING_METHOD_UNIFORM;
    gm2_set_line_height(raw_line_height);
    gm2_set_baseline(raw_baseline);
    return S_OK;
}

API Real gm2_set_origin(Real raw_origin_x, Real raw_origin_y) noexcept {
    draw.option().origin = { saturating_cast<f32>(raw_origin_x), saturating_cast<f32>(raw_origin_y) };
    return S_OK;
}

Eigen::Affine2f to_affine(Real raw_a00, Real raw_a01, Real raw_a10, Real raw_a11, Real raw_dx, Real raw_dy) noexcept {
    Eigen::Affine2f result;
    result.linear() = Eigen::Matrix2f{ { saturating_cast<f32>(raw_a00), saturating_cast<f32>(raw_a01) },
                                       { saturating_cast<f32>(raw_a10), saturating_cast<f32>(raw_a11) } };
    result.translation() = Eigen::Vector2f{ saturating_cast<f32>(raw_dx), saturating_cast<f32>(raw_dy) };
    return result;
}

API Real gm2_set_transform(Real raw_a00, Real raw_a01, Real raw_a10, Real raw_a11, Real raw_dx, Real raw_dy) noexcept {
    draw.option().transform = to_affine(raw_a00, raw_a01, raw_a10, raw_a11, raw_dx, raw_dy);
    return S_OK;
}

API Real gm2_add_transform(Real raw_a00, Real raw_a01, Real raw_a10, Real raw_a11, Real raw_dx, Real raw_dy) noexcept {
    draw.option().transform = to_affine(raw_a00, raw_a01, raw_a10, raw_a11, raw_dx, raw_dy) * draw.option().transform;
    return S_OK;
}

API Real gm2_add_translation(Real raw_x, Real raw_y) noexcept {
    draw.option().transform.pretranslate(Eigen::Vector2f{ saturating_cast<f32>(raw_x), saturating_cast<f32>(raw_y) });
    return S_OK;
}

API Real gm2_add_translation_x(Real raw_x) noexcept {
    return gm2_add_translation(raw_x, 0);
}

API Real gm2_add_translation_y(Real raw_y) noexcept {
    return gm2_add_translation(0, raw_y);
}

API Real gm2_add_scale(Real raw_x, Real raw_y) noexcept {
    draw.option().transform.prescale(Eigen::Vector2f{ saturating_cast<f32>(raw_x), saturating_cast<f32>(raw_y) });
    return S_OK;
}

API Real gm2_add_scale_x(Real raw_x) noexcept {
    return gm2_add_scale(raw_x, 1);
}

API Real gm2_add_scale_y(Real raw_y) noexcept {
    return gm2_add_scale(1, raw_y);
}

API Real gm2_add_rotation(Real raw_theta) noexcept {
    draw.option().transform.prerotate(Eigen::Rotation2Df{ -deg_to_rad(saturating_cast<f32>(raw_theta)) });
    return S_OK;
}

API Real gm2_add_skew(Real raw_x, Real raw_y) noexcept {
    draw.option().transform.preshear(
        std::tan(deg_to_rad(saturating_cast<f32>(raw_y))), std::tan(deg_to_rad(saturating_cast<f32>(raw_x)))
    );
    return S_OK;
}

API Real gm2_add_skew_x(Real raw_x) noexcept {
    return gm2_add_skew(raw_x, 0);
}

API Real gm2_add_skew_y(Real raw_y) noexcept {
    return gm2_add_skew(0, raw_y);
}

API Real gm2_get_aabb_x(StringRef raw_text) noexcept try { return draw.text_aabb(to_wstring(raw_text)).min().x(); }
GM_CATCH_RETURN()

API Real gm2_get_aabb_y(StringRef raw_text) noexcept try { return draw.text_aabb(to_wstring(raw_text)).min().y(); }
GM_CATCH_RETURN()

API Real gm2_get_aabb_width(StringRef raw_text) noexcept try {
    return draw.text_aabb(to_wstring(raw_text)).sizes().x();
}
GM_CATCH_RETURN()

API Real gm2_get_aabb_height(StringRef raw_text) noexcept try {
    return draw.text_aabb(to_wstring(raw_text)).sizes().y();
}
GM_CATCH_RETURN()

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
    return draw.option().max_size.x();
}

API Real gm2_get_max_height() noexcept {
    return draw.option().max_size.y();
}

API StringRef gm2_get_font() noexcept {
    usize id{ draw.option().font.second };
    string_value = id == 0 ? String{} : String{ id_map.at(id) };
    return string_value.data();
}

API StringRef gm2_get_font_name() noexcept {
    Font* font{ draw.option().font.first };
    string_value = font == nullptr ? String{} : String{ to_string(font->desc().name) };
    return string_value.data();
}

API Real gm2_get_font_weight() noexcept {
    Font* font{ draw.option().font.first };
    return font == nullptr ? -1 : font->desc().properties & FontDesc::WEIGHT_MASK;
}

API Real gm2_get_font_style() noexcept {
    Font* font{ draw.option().font.first };
    return font == nullptr ? -1 : font->desc().properties & FontDesc::STYLE_MASK;
}

API Real gm2_get_font_stretch() noexcept {
    Font* font{ draw.option().font.first };
    return font == nullptr ? -1 : font->desc().properties & FontDesc::STRETCH_MASK;
}

API Real gm2_get_font_size() noexcept {
    Font* font{ draw.option().font.first };
    return font == nullptr ? -1 : font->desc().size;
}

API StringRef gm2_get_font_locale() noexcept {
    Font* font{ draw.option().font.first };
    string_value = font == nullptr ? String{} : String{ to_string(font->desc().locale) };
    return string_value.data();
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

API Real gm2_get_origin_x() noexcept {
    return draw.option().origin.x();
}

API Real gm2_get_origin_y() noexcept {
    return draw.option().origin.y();
}

API Real gm2_get_transform_a00() noexcept {
    return draw.option().transform(0, 0);
}

API Real gm2_get_transform_a01() noexcept {
    return draw.option().transform(0, 1);
}

API Real gm2_get_transform_a10() noexcept {
    return draw.option().transform(1, 0);
}

API Real gm2_get_transform_a11() noexcept {
    return draw.option().transform(1, 1);
}

API Real gm2_get_transform_dx() noexcept {
    return draw.option().transform.translation().x();
}

API Real gm2_get_transform_dy() noexcept {
    return draw.option().transform.translation().y();
}
