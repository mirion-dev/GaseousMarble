#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

#include <cassert>

import std;
import gm;

using namespace gm;

static String string_value;

static std::unordered_map<std::string, Font> font_map;
static std::unordered_map<usize, std::string_view> id_map;
static Draw draw{ 1024 };

API Real gm_font(StringRef raw_key, StringRef raw_sprite_path) noexcept {
    std::string key{ raw_key };
    if (key.empty()) {
        return -100; // Invalid argument
    }

    try {
        auto [iter, inserted]{ font_map.try_emplace(std::move(key), raw_sprite_path) };
        if (!inserted) {
            return 1; // Font already exists
        }

        id_map.try_emplace(iter->second.id(), iter->first);
        return 0;
    } catch (const std::system_error& error) {
        assert(error.code().category() == font_error_category());
        return error.code().value();
    }
}

API Real gm_free(StringRef raw_key) noexcept {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        return 1; // Font not found
    }

    if (draw.option().font.second == iter->second.id()) {
        draw.option().font = {};
    }

    id_map.erase(iter->second.id());
    font_map.erase(iter);
    return 0;
}

API Real gm_clear() noexcept {
    draw.option().font = {};
    id_map.clear();
    font_map.clear();
    return 0;
}

API Real gm_draw(Real raw_x, Real raw_y, StringRef raw_text) noexcept {
    try {
        draw.text(saturating_cast<f32>(raw_x), saturating_cast<f32>(raw_y), raw_text);
        return 0;
    } catch (const std::system_error& error) {
        assert(error.code().category() == layout_error_category());
        return error.code().value();
    }
}

API Real gm_width(StringRef raw_text) noexcept {
    try {
        return draw.text_width(raw_text);
    } catch (const std::system_error& error) {
        assert(error.code().category() == layout_error_category());
        return error.code().value();
    }
}

API Real gm_height(StringRef raw_text) noexcept {
    try {
        return draw.text_height(raw_text);
    } catch (const std::system_error& error) {
        assert(error.code().category() == layout_error_category());
        return error.code().value();
    }
}

API Real gm_set_font(StringRef raw_key) noexcept {
    auto iter{ font_map.find(raw_key) };
    if (iter == font_map.end()) {
        return -1; // Font not found
    }

    draw.option().font = { &iter->second, iter->second.id() };
    return 0;
}

API Real gm_set_halign(Real raw_halign) noexcept {
    draw.option().halign = saturating_cast<i8>(raw_halign);
    return 0;
}

API Real gm_set_valign(Real raw_valign) noexcept {
    draw.option().valign = saturating_cast<i8>(raw_valign);
    return 0;
}

API Real gm_set_justified(Real raw_justified) noexcept {
    draw.option().justified = static_cast<bool>(raw_justified);
    return 0;
}

API Real gm_set_align(Real raw_halign, Real raw_valign) noexcept {
    gm_set_halign(raw_halign);
    gm_set_valign(raw_valign);
    return 0;
}

API Real gm_set_align3(Real raw_halign, Real raw_valign, Real raw_justified) noexcept {
    gm_set_align(raw_halign, raw_valign);
    gm_set_justified(raw_justified);
    return 0;
}

API Real gm_set_color2(Real raw_color_top, Real raw_color_bottom) noexcept {
    draw.option().color_top = saturating_cast<u32>(raw_color_top);
    draw.option().color_bottom = saturating_cast<u32>(raw_color_bottom);
    return 0;
}

API Real gm_set_color(Real raw_color) noexcept {
    gm_set_color2(raw_color, raw_color);
    return 0;
}

API Real gm_set_alpha(Real raw_alpha) noexcept {
    draw.option().alpha = saturating_cast<f32>(raw_alpha);
    return 0;
}

API Real gm_set_letter_spacing(Real raw_letter_spacing) noexcept {
    draw.option().letter_spacing = saturating_cast<f32>(raw_letter_spacing);
    return 0;
}

API Real gm_set_word_spacing(Real raw_word_spacing) noexcept {
    draw.option().word_spacing = saturating_cast<f32>(raw_word_spacing);
    return 0;
}

API Real gm_set_paragraph_spacing(Real raw_paragraph_spacing) noexcept {
    draw.option().paragraph_spacing = saturating_cast<f32>(raw_paragraph_spacing);
    return 0;
}

API Real gm_set_line_height(Real raw_line_height) noexcept {
    draw.option().line_height = saturating_cast<f32>(raw_line_height);
    return 0;
}

API Real gm_set_max_line_length(Real raw_max_line_length) noexcept {
    draw.option().max_line_length = std::max(saturating_cast<f32>(raw_max_line_length), 0.f);
    return 0;
}

API Real gm_set_offset(Real raw_x, Real raw_y) noexcept {
    draw.option().offset_x = saturating_cast<f32>(raw_x);
    draw.option().offset_y = saturating_cast<f32>(raw_y);
    return 0;
}

API Real gm_set_scale(Real raw_x, Real raw_y) noexcept {
    f32 x{ saturating_cast<f32>(raw_x) };
    f32 y{ saturating_cast<f32>(raw_y) };
    if (x <= 0 || y <= 0) {
        return -1; // Invalid argument
    }

    draw.option().scale_x = x;
    draw.option().scale_y = y;
    return 0;
}

API Real gm_set_rotation(Real raw_rotation) noexcept {
    draw.option().rotation = saturating_cast<f32>(raw_rotation);
    return 0;
}

API StringRef gm_get_font() noexcept {
    usize id{ draw.option().font.second };
    string_value = id == 0 ? String{} : String{ id_map.at(id) };
    return string_value.data();
}

API Real gm_get_halign() noexcept {
    return draw.option().halign;
}

API Real gm_get_valign() noexcept {
    return draw.option().valign;
}

API Real gm_is_justified() noexcept {
    return draw.option().justified;
}

API Real gm_get_color_top() noexcept {
    return draw.option().color_top;
}

API Real gm_get_color_bottom() noexcept {
    return draw.option().color_bottom;
}

API Real gm_get_alpha() noexcept {
    return draw.option().alpha;
}

API Real gm_get_letter_spacing() noexcept {
    return draw.option().letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return draw.option().word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return draw.option().paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return draw.option().line_height;
}

API Real gm_get_max_line_length() noexcept {
    return draw.option().max_line_length;
}

API Real gm_get_offset_x() noexcept {
    return draw.option().offset_x;
}

API Real gm_get_offset_y() noexcept {
    return draw.option().offset_y;
}

API Real gm_get_scale_x() noexcept {
    return draw.option().scale_x;
}

API Real gm_get_scale_y() noexcept {
    return draw.option().scale_y;
}

API Real gm_get_rotation() noexcept {
    return draw.option().rotation;
}
