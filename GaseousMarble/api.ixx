#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

static std::unordered_map<std::string, Font> font_map;
static TextOption text_option;
static DrawOption draw_option;
static TextCache text_cache{ 1024 };

API Real gm_font(StringRef raw_font_name, StringRef raw_sprite_path) noexcept {
    std::string font_name{ raw_font_name };
    if (font_name.empty()) {
        return -100; // invalid argument
    }

    try {
        // font already exists
        return font_map.try_emplace(std::move(font_name), raw_font_name, raw_sprite_path).second ? 0 : 1;
    }
    catch (FontError error) {
        return static_cast<int>(error);
    }
}

API Real gm_free(StringRef raw_font_name) noexcept {
    auto iter{ font_map.find(raw_font_name) };
    if (iter == font_map.end()) {
        return 1; // font not found
    }

    if (text_option.font == &iter->second) {
        text_option.font = {};
        text_cache.clear();
    }

    font_map.erase(iter);
    return 0;
}

API Real gm_clear() noexcept {
    text_option.font = {};
    text_cache.clear();
    font_map.clear();
    return 0;
}

API Real gm_draw(Real raw_x, Real raw_y, StringRef raw_str) noexcept {
    try {
        text_cache.get(raw_str, text_option).draw(static_cast<f32>(raw_x), static_cast<f32>(raw_y), draw_option);
        return 0;
    }
    catch (TextError error) {
        return static_cast<int>(error);
    }
}

API Real gm_width(StringRef raw_str) noexcept {
    try {
        return text_cache.get(raw_str, text_option).width();
    }
    catch (TextError error) {
        return static_cast<int>(error);
    }
}

API Real gm_height(StringRef raw_str) noexcept {
    try {
        return text_cache.get(raw_str, text_option).height();
    }
    catch (TextError error) {
        return static_cast<int>(error);
    }
}

API Real gm_set_font(StringRef raw_font_name) noexcept {
    auto iter{ font_map.find(raw_font_name) };
    if (iter == font_map.end()) {
        return -1; // font not found
    }

    if (text_option.font != &iter->second) {
        text_option.font = &iter->second;
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_halign(Real raw_align) noexcept {
    draw_option.halign = static_cast<i8>(raw_align);
    return 0;
}

API Real gm_set_valign(Real raw_align) noexcept {
    draw_option.valign = static_cast<i8>(raw_align);
    return 0;
}

API Real gm_set_justified(Real raw_justified) noexcept {
    draw_option.justified = static_cast<bool>(raw_justified);
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
    draw_option.color_top = static_cast<u32>(raw_color_top);
    draw_option.color_bottom = static_cast<u32>(raw_color_bottom);
    return 0;
}

API Real gm_set_color(Real raw_color) noexcept {
    gm_set_color2(raw_color, raw_color);
    return 0;
}

API Real gm_set_alpha(Real raw_alpha) noexcept {
    draw_option.alpha = static_cast<f32>(raw_alpha);
    return 0;
}

API Real gm_set_letter_spacing(Real raw_spacing) noexcept {
    if (text_option.letter_spacing != static_cast<f32>(raw_spacing)) {
        text_option.letter_spacing = static_cast<f32>(raw_spacing);
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_word_spacing(Real raw_spacing) noexcept {
    if (text_option.word_spacing != static_cast<f32>(raw_spacing)) {
        text_option.word_spacing = static_cast<f32>(raw_spacing);
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_paragraph_spacing(Real raw_spacing) noexcept {
    if (text_option.paragraph_spacing != static_cast<f32>(raw_spacing)) {
        text_option.paragraph_spacing = static_cast<f32>(raw_spacing);
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_line_height(Real raw_height) noexcept {
    if (text_option.line_height != static_cast<f32>(raw_height)) {
        text_option.line_height = static_cast<f32>(raw_height);
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_max_line_length(Real raw_length) noexcept {
    raw_length = std::max(raw_length, 0.);
    if (text_option.max_line_length != static_cast<f32>(raw_length)) {
        text_option.max_line_length = static_cast<f32>(raw_length);
        text_cache.clear();
    }
    return 0;
}

API Real gm_set_offset(Real raw_x, Real raw_y) noexcept {
    draw_option.offset_x = static_cast<f32>(raw_x);
    draw_option.offset_y = static_cast<f32>(raw_y);
    return 0;
}

API Real gm_set_scale(Real raw_x, Real raw_y) noexcept {
    if (raw_x <= 0 || raw_y <= 0) {
        return -1; // invalid argument(s)
    }

    draw_option.scale_x = static_cast<f32>(raw_x);
    draw_option.scale_y = static_cast<f32>(raw_y);
    return 0;
}

API Real gm_set_rotation(Real raw_theta) noexcept {
    draw_option.rotation = static_cast<f32>(raw_theta);
    return 0;
}

API StringRef gm_get_font() noexcept {
    return text_option.font != nullptr ? text_option.font->name().data() : ""; // font unspecified
}

API Real gm_get_halign() noexcept {
    return draw_option.halign;
}

API Real gm_get_valign() noexcept {
    return draw_option.valign;
}

API Real gm_is_justified() noexcept {
    return draw_option.justified;
}

API Real gm_get_color_top() noexcept {
    return draw_option.color_top;
}

API Real gm_get_color_bottom() noexcept {
    return draw_option.color_bottom;
}

API Real gm_get_alpha() noexcept {
    return draw_option.alpha;
}

API Real gm_get_letter_spacing() noexcept {
    return text_option.letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return text_option.word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return text_option.paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return text_option.line_height;
}

API Real gm_get_max_line_length() noexcept {
    return text_option.max_line_length;
}

API Real gm_get_offset_x() noexcept {
    return draw_option.offset_x;
}

API Real gm_get_offset_y() noexcept {
    return draw_option.offset_y;
}

API Real gm_get_scale_x() noexcept {
    return draw_option.scale_x;
}

API Real gm_get_scale_y() noexcept {
    return draw_option.scale_y;
}

API Real gm_get_rotation() noexcept {
    return draw_option.rotation;
}
