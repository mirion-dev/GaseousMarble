#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

static std::unordered_map<std::string, Font, Hash, std::equal_to<>> font_map;
static Text::Option option;
static Text::DrawOption draw_option;
static Cache<std::string, Text, 1024> cache;

API Real gm_font(StringView font_name, StringView sprite_path) noexcept {
    if (font_name.empty()) {
        return -100; // invalid argument
    }

    try {
        // font already exists
        return font_map.try_emplace(std::string{ font_name }, font_name, sprite_path).second ? 0 : 1;
    }
    catch (Font::Error error) {
        return static_cast<int>(error);
    }
}

API Real gm_free(StringView font_name) noexcept {
    auto iter{ font_map.find(std::string_view{ font_name }) };
    if (iter == font_map.end()) {
        return 1; // font not found
    }

    if (option.font == &iter->second) {
        option.font = {};
        cache.clear();
    }

    font_map.erase(iter);
    return 0;
}

API Real gm_clear() noexcept {
    option.font = {};
    cache.clear();
    font_map.clear();
    return 0;
}

API Real gm_draw(Real x, Real y, StringView str) noexcept {
    try {
        cache.try_emplace(std::string{ str }, str, option).first->second
            .draw(static_cast<f32>(x), static_cast<f32>(y), draw_option);
        return 0;
    }
    catch (Text::Error error) {
        return static_cast<int>(error);
    }
}

API Real gm_width(StringView str) noexcept {
    try {
        return cache.try_emplace(std::string{ str }, str, option).first->second.width();
    }
    catch (Text::Error error) {
        return static_cast<int>(error);
    }
}

API Real gm_height(StringView str) noexcept {
    try {
        return cache.try_emplace(std::string{ str }, str, option).first->second.height();
    }
    catch (Text::Error error) {
        return static_cast<int>(error);
    }
}

API Real gm_set_font(StringView font_name) noexcept {
    auto iter{ font_map.find(std::string_view{ font_name }) };
    if (iter == font_map.end()) {
        return -1; // font not found
    }

    if (option.font != &iter->second) {
        option.font = &iter->second;
        cache.clear();
    }
    return 0;
}

API Real gm_set_halign(Real align) noexcept {
    draw_option.halign = static_cast<i8>(align);
    return 0;
}

API Real gm_set_valign(Real align) noexcept {
    draw_option.valign = static_cast<i8>(align);
    return 0;
}

API Real gm_set_justified(Real justified) noexcept {
    draw_option.justified = static_cast<bool>(justified);
    return 0;
}

API Real gm_set_align(Real halign, Real valign) noexcept {
    gm_set_halign(halign);
    gm_set_valign(valign);
    return 0;
}

API Real gm_set_align3(Real halign, Real valign, Real justified) noexcept {
    gm_set_align(halign, valign);
    gm_set_justified(justified);
    return 0;
}

API Real gm_set_color2(Real color_top, Real color_bottom) noexcept {
    draw_option.color_top = static_cast<u32>(color_top);
    draw_option.color_bottom = static_cast<u32>(color_bottom);
    return 0;
}

API Real gm_set_color(Real color) noexcept {
    gm_set_color2(color, color);
    return 0;
}

API Real gm_set_alpha(Real alpha) noexcept {
    draw_option.alpha = static_cast<f32>(alpha);
    return 0;
}

API Real gm_set_letter_spacing(Real spacing) noexcept {
    if (option.letter_spacing != static_cast<f32>(spacing)) {
        option.letter_spacing = static_cast<f32>(spacing);
        cache.clear();
    }
    return 0;
}

API Real gm_set_word_spacing(Real spacing) noexcept {
    if (option.word_spacing != static_cast<f32>(spacing)) {
        option.word_spacing = static_cast<f32>(spacing);
        cache.clear();
    }
    return 0;
}

API Real gm_set_paragraph_spacing(Real spacing) noexcept {
    if (option.paragraph_spacing != static_cast<f32>(spacing)) {
        option.paragraph_spacing = static_cast<f32>(spacing);
        cache.clear();
    }
    return 0;
}

API Real gm_set_line_height(Real height) noexcept {
    if (option.line_height != static_cast<f32>(height)) {
        option.line_height = static_cast<f32>(height);
        cache.clear();
    }
    return 0;
}

API Real gm_set_max_line_length(Real length) noexcept {
    length = std::max(length, 0.);
    if (option.max_line_length != static_cast<f32>(length)) {
        option.max_line_length = static_cast<f32>(length);
        cache.clear();
    }
    return 0;
}

API Real gm_set_offset(Real x, Real y) noexcept {
    draw_option.offset_x = static_cast<f32>(x);
    draw_option.offset_y = static_cast<f32>(y);
    return 0;
}

API Real gm_set_scale(Real x, Real y) noexcept {
    if (x <= 0 || y <= 0) {
        return -1; // invalid argument(s)
    }

    draw_option.scale_x = static_cast<f32>(x);
    draw_option.scale_y = static_cast<f32>(y);
    return 0;
}

API Real gm_set_rotation(Real theta) noexcept {
    draw_option.rotation = static_cast<f32>(theta);
    return 0;
}

API StringView gm_get_font() noexcept {
    return option.font != nullptr ? option.font->name() : ""; // font unspecified
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
    return option.letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return option.word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return option.paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return option.line_height;
}

API Real gm_get_max_line_length() noexcept {
    return option.max_line_length;
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
