export module gm;

import std;
import gm.core;
import gm.engine;
import gm.old.draw;

using namespace gm::core;
using namespace gm::engine;

#ifdef GASEOUSMARBLE_EXPORTS
#define GASEOUSMARBLE_API __declspec(dllexport)
#else
#define GASEOUSMARBLE_API __declspec(dllimport)
#endif

extern "C" {

    GASEOUSMARBLE_API Real gm_font(StringView sprite_path, StringView glyph_path) noexcept;
    GASEOUSMARBLE_API Real gm_width(StringView text) noexcept;
    GASEOUSMARBLE_API Real gm_height(StringView text) noexcept;
    GASEOUSMARBLE_API Real gm_draw(Real x, Real y, StringView text) noexcept;
    GASEOUSMARBLE_API Real gm_free(Real font_id) noexcept;
    GASEOUSMARBLE_API Real gm_clear() noexcept;

    GASEOUSMARBLE_API Real gm_set_font(Real font_id) noexcept;
    GASEOUSMARBLE_API Real gm_set_color(Real color) noexcept;
    GASEOUSMARBLE_API Real gm_set_color2(Real color_top, Real color_bottom) noexcept;
    GASEOUSMARBLE_API Real gm_set_alpha(Real alpha) noexcept;
    GASEOUSMARBLE_API Real gm_set_halign(Real align) noexcept;
    GASEOUSMARBLE_API Real gm_set_valign(Real align) noexcept;
    GASEOUSMARBLE_API Real gm_set_align(Real halign, Real valign) noexcept;
    GASEOUSMARBLE_API Real gm_set_max_line_width(Real max_width) noexcept;
    GASEOUSMARBLE_API Real gm_set_letter_spacing(Real spacing) noexcept;
    GASEOUSMARBLE_API Real gm_set_word_spacing(Real spacing) noexcept;
    GASEOUSMARBLE_API Real gm_set_line_height(Real height) noexcept;
    GASEOUSMARBLE_API Real gm_set_offset(Real x, Real y) noexcept;
    GASEOUSMARBLE_API Real gm_set_scale(Real x, Real y) noexcept;

    GASEOUSMARBLE_API Real gm_get_font() noexcept;
    GASEOUSMARBLE_API Real gm_get_color_top() noexcept;
    GASEOUSMARBLE_API Real gm_get_color_bottom() noexcept;
    GASEOUSMARBLE_API Real gm_get_alpha() noexcept;
    GASEOUSMARBLE_API Real gm_get_halign() noexcept;
    GASEOUSMARBLE_API Real gm_get_valign() noexcept;
    GASEOUSMARBLE_API Real gm_get_max_line_width() noexcept;
    GASEOUSMARBLE_API Real gm_get_letter_spacing() noexcept;
    GASEOUSMARBLE_API Real gm_get_word_spacing() noexcept;
    GASEOUSMARBLE_API Real gm_get_line_height() noexcept;
    GASEOUSMARBLE_API Real gm_get_offset_x() noexcept;
    GASEOUSMARBLE_API Real gm_get_offset_y() noexcept;
    GASEOUSMARBLE_API Real gm_get_scale_x() noexcept;
    GASEOUSMARBLE_API Real gm_get_scale_y() noexcept;

}