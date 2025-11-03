#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

std::unordered_map<std::u8string, Font> font_map;
Draw draw;

API Real gm_font(StringView font_name, StringView sprite_path) noexcept {
    if (font_map.contains(std::u8string{ font_name })) {
        return 1; // font already exists
    }

    try {
        font_map.emplace(font_name, Font{ font_name, sprite_path });
    }
    catch (Font::Error error) {
        return static_cast<int>(error);
    }

    return 0;
}

API Real gm_free(StringView font_name) noexcept {
    return font_map.erase(std::u8string{ font_name }) ? 0 : 1; // font not found
}

API Real gm_clear() noexcept {
    font_map.clear();
    return 0;
}

API Real gm_draw(Real x, Real y, StringView str) noexcept {
    auto res_text{ draw.create_text(str) };
    if (!res_text) {
        return static_cast<int>(res_text.error());
    }

    auto error{ draw.text(x, y, res_text->result) };
    if (error != Draw::Error::no_error) {
        return static_cast<int>(error);
    }

    return static_cast<int>(res_text->warning);
}

API Real gm_width(StringView str) noexcept {
    auto res_text{ draw.create_text(str) };
    if (!res_text) {
        return static_cast<int>(res_text.error());
    }

    return res_text->result.layout.width;
}

API Real gm_height(StringView text) noexcept {
    auto res_text{ draw.create_text(text) };
    if (!res_text) {
        return static_cast<int>(res_text.error());
    }

    return res_text->result.layout.height;
}

API Real gm_set_font(StringView font_name) noexcept {
    auto iter{ font_map.find(std::u8string{ font_name }) };
    if (iter == font_map.end()) {
        return -1; // font not found
    }

    draw.option.font = &iter->second;
    return 0;
}

API Real gm_set_halign(Real align) noexcept {
    draw.option.halign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return 0;
}

API Real gm_set_valign(Real align) noexcept {
    draw.option.valign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return 0;
}

API Real gm_set_justified(Real justified) noexcept {
    draw.option.justified = justified;
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
    draw.option.color_top = static_cast<u32>(color_top);
    draw.option.color_bottom = static_cast<u32>(color_bottom);
    return 0;
}

API Real gm_set_color(Real color) noexcept {
    gm_set_color2(color, color);
    return 0;
}

API Real gm_set_alpha(Real alpha) noexcept {
    if (alpha < 0 || alpha > 1) {
        return -1; // invalid argument
    }

    draw.option.alpha = alpha;
    return 0;
}

API Real gm_set_letter_spacing(Real spacing) noexcept {
    draw.option.letter_spacing = spacing;
    return 0;
}

API Real gm_set_word_spacing(Real spacing) noexcept {
    draw.option.word_spacing = spacing;
    return 0;
}

API Real gm_set_paragraph_spacing(Real spacing) noexcept {
    draw.option.paragraph_spacing = spacing;
    return 0;
}

API Real gm_set_line_height(Real height) noexcept {
    if (height <= 0) {
        return -1; // invalid argument
    }

    draw.option.line_height = height;
    return 0;
}

API Real gm_set_max_line_length(Real length) noexcept {
    if (length < 0) {
        return -1; // invalid argument
    }

    draw.option.max_line_length = length;
    return 0;
}

API Real gm_set_offset(Real x, Real y) noexcept {
    draw.option.offset_x = x;
    draw.option.offset_y = y;
    return 0;
}

API Real gm_set_scale(Real x, Real y) noexcept {
    if (x <= 0 || y <= 0) {
        return -1; // invalid argument(s)
    }

    draw.option.scale_x = x;
    draw.option.scale_y = y;
    return 0;
}

API Real gm_set_rotation(Real theta) noexcept {
    draw.option.rotation = theta;
    return 0;
}

API StringView gm_get_font() noexcept {
    return draw.option.font->name();
}

API Real gm_get_halign() noexcept {
    return draw.option.halign;
}

API Real gm_get_valign() noexcept {
    return draw.option.valign;
}

API Real gm_is_justified() noexcept {
    return draw.option.justified;
}

API Real gm_get_color_top() noexcept {
    return draw.option.color_top;
}

API Real gm_get_color_bottom() noexcept {
    return draw.option.color_bottom;
}

API Real gm_get_alpha() noexcept {
    return draw.option.alpha;
}

API Real gm_get_letter_spacing() noexcept {
    return draw.option.letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return draw.option.word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return draw.option.paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return draw.option.line_height;
}

API Real gm_get_max_line_length() noexcept {
    return draw.option.max_line_length;
}

API Real gm_get_offset_x() noexcept {
    return draw.option.offset_x;
}

API Real gm_get_offset_y() noexcept {
    return draw.option.offset_y;
}

API Real gm_get_scale_x() noexcept {
    return draw.option.scale_x;
}

API Real gm_get_scale_y() noexcept {
    return draw.option.scale_y;
}

API Real gm_get_rotation() noexcept {
    return draw.option.rotation;
}
