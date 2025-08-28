#ifdef GASEOUSMARBLE_EXPORTS
#define API extern "C" __declspec(dllexport)
#else
#define API extern "C" __declspec(dllimport)
#endif

import std;
import gm;

using namespace gm;

std::unordered_map<std::string, Font> font_map;
Draw draw;

// reserve for the `next` branch
API Real gm_init() noexcept {
    return 0;
}

API Real gm_font(StringView font_name, StringView sprite_path) noexcept {
    if (font_map.contains(std::string{ font_name })) {
        return 1; // font already exists
    }

    Font font;
    try {
        font = { font_name, sprite_path };
    }
    catch (const std::ios_base::failure&) {
        return -1;
    }
    catch (const Font::InvalidHeaderError&) {
        return -2;
    }
    catch (const Font::DataCorruptionError&) {
        return -3;
    }
    catch (const Font::SpriteAddFailure&) {
        return -4;
    }

    font_map.emplace(font_name, std::move(font));
    return 0;
}

API Real gm_free(StringView font_name) noexcept {
    return font_map.erase(std::string{ font_name }) ? 0 : 1; // font not found
}

API Real gm_clear() noexcept {
    font_map.clear();
    return 0;
}

API Real gm_draw(Real x, Real y, StringView text) noexcept {
    auto exp_warning{ draw.text(x, y, text) };
    if (!exp_warning) {
        switch (exp_warning.error()) {
        case Draw::Error::invalid_encoding:
            return -1;
        case Draw::Error::tokenization_failed:
            return -2;
        case Draw::Error::font_not_set:
            return -3;
        default:
            std::unreachable();
        }
    }

    switch (exp_warning->warning) {
    case Draw::Warning::no_warning:
        return 0;
    case Draw::Warning::missing_glyphs:
        return 1;
    default:
        std::unreachable();
    }
}

API Real gm_width(StringView text) noexcept {
    auto exp_metrics{ draw.measure(text) };
    if (!exp_metrics) {
        switch (exp_metrics.error()) {
        case Draw::Error::invalid_encoding:
            return -1;
        case Draw::Error::tokenization_failed:
            return -2;
        default:
            std::unreachable();
        }
    }

    return exp_metrics->result.width;
}

API Real gm_height(StringView text) noexcept {
    auto exp_metrics{ draw.measure(text) };
    if (!exp_metrics) {
        switch (exp_metrics.error()) {
        case Draw::Error::invalid_encoding:
            return -1;
        case Draw::Error::tokenization_failed:
            return -2;
        default:
            std::unreachable();
        }
    }

    return exp_metrics->result.height;
}

API Real gm_set_font(StringView font_name) noexcept {
    auto iter{ font_map.find(std::string{ font_name }) };
    if (iter == font_map.end()) {
        return -1; // font not found
    }

    draw.setting.font = &iter->second;
    return 0;
}

API Real gm_set_halign(Real align) noexcept {
    draw.setting.halign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return 0;
}

API Real gm_set_valign(Real align) noexcept {
    draw.setting.valign = align == 0 ? 0 : align < 0 ? -1 : 1;
    return 0;
}

API Real gm_set_justified(Real justified) noexcept {
    draw.setting.justified = justified;
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
    draw.setting.color_top = static_cast<u32>(color_top);
    draw.setting.color_bottom = static_cast<u32>(color_bottom);
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

    draw.setting.alpha = alpha;
    return 0;
}

API Real gm_set_letter_spacing(Real spacing) noexcept {
    draw.setting.letter_spacing = spacing;
    return 0;
}

API Real gm_set_word_spacing(Real spacing) noexcept {
    draw.setting.word_spacing = spacing;
    return 0;
}

API Real gm_set_paragraph_spacing(Real spacing) noexcept {
    draw.setting.paragraph_spacing = spacing;
    return 0;
}

API Real gm_set_line_height(Real height) noexcept {
    if (height <= 0) {
        return -1; // invalid argument
    }

    draw.setting.line_height = height;
    return 0;
}

API Real gm_set_max_line_length(Real length) noexcept {
    if (length < 0) {
        return -1; // invalid argument
    }

    draw.setting.max_line_length = length;
    return 0;
}

API Real gm_set_offset(Real x, Real y) noexcept {
    draw.setting.offset_x = x;
    draw.setting.offset_y = y;
    return 0;
}

API Real gm_set_scale(Real x, Real y) noexcept {
    if (x <= 0 || y <= 0) {
        return -1; // invalid argument(s)
    }

    draw.setting.scale_x = x;
    draw.setting.scale_y = y;
    return 0;
}

API Real gm_set_rotation(Real theta) noexcept {
    draw.setting.rotation = theta;
    return 0;
}

// MSVC will unhappy if I use String as the return type
API const char* gm_get_font() noexcept {
    return draw.setting.font->name().data();
}

API Real gm_get_halign() noexcept {
    return draw.setting.halign;
}

API Real gm_get_valign() noexcept {
    return draw.setting.valign;
}

API Real gm_is_justified() noexcept {
    return draw.setting.justified;
}

API Real gm_get_color_top() noexcept {
    return draw.setting.color_top;
}

API Real gm_get_color_bottom() noexcept {
    return draw.setting.color_bottom;
}

API Real gm_get_alpha() noexcept {
    return draw.setting.alpha;
}

API Real gm_get_letter_spacing() noexcept {
    return draw.setting.letter_spacing;
}

API Real gm_get_word_spacing() noexcept {
    return draw.setting.word_spacing;
}

API Real gm_get_paragraph_spacing() noexcept {
    return draw.setting.paragraph_spacing;
}

API Real gm_get_line_height() noexcept {
    return draw.setting.line_height;
}

API Real gm_get_max_line_length() noexcept {
    return draw.setting.max_line_length;
}

API Real gm_get_offset_x() noexcept {
    return draw.setting.offset_x;
}

API Real gm_get_offset_y() noexcept {
    return draw.setting.offset_y;
}

API Real gm_get_scale_x() noexcept {
    return draw.setting.scale_x;
}

API Real gm_get_scale_y() noexcept {
    return draw.setting.scale_y;
}

API Real gm_get_rotation() noexcept {
    return draw.setting.rotation;
}
