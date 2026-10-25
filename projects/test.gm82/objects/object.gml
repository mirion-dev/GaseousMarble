#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dll_path{ dll_path = parameter_string(1) }
global.gm2_internal_to_real         = external_define(dll_path, "gm2_internal_to_real", dll_cdecl, ty_real, 1, ty_string)
global.gm2_internal_new_font        = external_define(dll_path, "gm2_internal_new_font", dll_cdecl, ty_real, 5, ty_real, ty_real, ty_real, ty_real, ty_real)
global.gm2_draw_text                = external_define(dll_path, "gm2_draw_text", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)
global.gm2_get_aabb_x               = external_define(dll_path, "gm2_get_aabb_x", dll_cdecl, ty_real, 1, ty_string)
global.gm2_get_aabb_y               = external_define(dll_path, "gm2_get_aabb_y", dll_cdecl, ty_real, 1, ty_string)
global.gm2_get_aabb_width           = external_define(dll_path, "gm2_get_aabb_width", dll_cdecl, ty_real, 1, ty_string)
global.gm2_get_aabb_height          = external_define(dll_path, "gm2_get_aabb_height", dll_cdecl, ty_real, 1, ty_string)
global.gm2_save_options             = external_define(dll_path, "gm2_save_options", dll_cdecl, ty_real, 1, ty_string)
global.gm2_load_options             = external_define(dll_path, "gm2_load_options", dll_cdecl, ty_real, 1, ty_string)
global.gm2_set_alignment            = external_define(dll_path, "gm2_set_alignment", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_text_alignment       = external_define(dll_path, "gm2_set_text_alignment", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_par_alignment        = external_define(dll_path, "gm2_set_par_alignment", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_word_wrapping        = external_define(dll_path, "gm2_set_word_wrapping", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_direction            = external_define(dll_path, "gm2_set_direction", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_text_direction       = external_define(dll_path, "gm2_set_text_direction", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_par_direction        = external_define(dll_path, "gm2_set_par_direction", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_tab_spacing          = external_define(dll_path, "gm2_set_tab_spacing", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_trimming             = external_define(dll_path, "gm2_set_trimming", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_max_size             = external_define(dll_path, "gm2_set_max_size", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_set_max_width            = external_define(dll_path, "gm2_set_max_width", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_max_height           = external_define(dll_path, "gm2_set_max_height", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_font                 = external_define(dll_path, "gm2_set_font", dll_cdecl, ty_real, 1, ty_string)
global.gm2_set_letter_spacing       = external_define(dll_path, "gm2_set_letter_spacing", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_line_spacing         = external_define(dll_path, "gm2_set_line_spacing", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_set_uniform_line_spacing = external_define(dll_path, "gm2_set_uniform_line_spacing", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_set_line_height          = external_define(dll_path, "gm2_set_line_height", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_baseline             = external_define(dll_path, "gm2_set_baseline", dll_cdecl, ty_real, 1, ty_real)
global.gm2_set_origin               = external_define(dll_path, "gm2_set_origin", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_set_transform            = external_define(dll_path, "gm2_set_transform", dll_cdecl, ty_real, 6, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real)
global.gm2_add_transform            = external_define(dll_path, "gm2_add_transform", dll_cdecl, ty_real, 6, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real)
global.gm2_add_translation          = external_define(dll_path, "gm2_add_translation", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_add_translation_x        = external_define(dll_path, "gm2_add_translation_x", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_translation_y        = external_define(dll_path, "gm2_add_translation_y", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_scale                = external_define(dll_path, "gm2_add_scale", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_add_scale_x              = external_define(dll_path, "gm2_add_scale_x", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_scale_y              = external_define(dll_path, "gm2_add_scale_y", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_rotation             = external_define(dll_path, "gm2_add_rotation", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_skew                 = external_define(dll_path, "gm2_add_skew", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm2_add_skew_x               = external_define(dll_path, "gm2_add_skew_x", dll_cdecl, ty_real, 1, ty_real)
global.gm2_add_skew_y               = external_define(dll_path, "gm2_add_skew_y", dll_cdecl, ty_real, 1, ty_real)
global.gm2_get_text_alignment       = external_define(dll_path, "gm2_get_text_alignment", dll_cdecl, ty_real, 0)
global.gm2_get_par_alignment        = external_define(dll_path, "gm2_get_par_alignment", dll_cdecl, ty_real, 0)
global.gm2_get_word_wrapping        = external_define(dll_path, "gm2_get_word_wrapping", dll_cdecl, ty_real, 0)
global.gm2_get_text_direction       = external_define(dll_path, "gm2_get_text_direction", dll_cdecl, ty_real, 0)
global.gm2_get_par_direction        = external_define(dll_path, "gm2_get_par_direction", dll_cdecl, ty_real, 0)
global.gm2_get_tab_spacing          = external_define(dll_path, "gm2_get_tab_spacing", dll_cdecl, ty_real, 0)
global.gm2_get_trimming             = external_define(dll_path, "gm2_get_trimming", dll_cdecl, ty_real, 0)
global.gm2_get_max_width            = external_define(dll_path, "gm2_get_max_width", dll_cdecl, ty_real, 0)
global.gm2_get_max_height           = external_define(dll_path, "gm2_get_max_height", dll_cdecl, ty_real, 0)
global.gm2_get_font                 = external_define(dll_path, "gm2_get_font", dll_cdecl, ty_string, 0)
global.gm2_get_font_name            = external_define(dll_path, "gm2_get_font_name", dll_cdecl, ty_string, 0)
global.gm2_get_font_size            = external_define(dll_path, "gm2_get_font_size", dll_cdecl, ty_real, 0)
global.gm2_get_font_weight          = external_define(dll_path, "gm2_get_font_weight", dll_cdecl, ty_real, 0)
global.gm2_get_font_style           = external_define(dll_path, "gm2_get_font_style", dll_cdecl, ty_real, 0)
global.gm2_get_font_stretch         = external_define(dll_path, "gm2_get_font_stretch", dll_cdecl, ty_real, 0)
global.gm2_get_font_locale          = external_define(dll_path, "gm2_get_font_locale", dll_cdecl, ty_string, 0)
global.gm2_get_letter_spacing       = external_define(dll_path, "gm2_get_letter_spacing", dll_cdecl, ty_real, 0)
global.gm2_get_line_spacing_type    = external_define(dll_path, "gm2_get_line_spacing_type", dll_cdecl, ty_real, 0)
global.gm2_get_line_height          = external_define(dll_path, "gm2_get_line_height", dll_cdecl, ty_real, 0)
global.gm2_get_baseline             = external_define(dll_path, "gm2_get_baseline", dll_cdecl, ty_real, 0)
global.gm2_get_origin_x             = external_define(dll_path, "gm2_get_origin_x", dll_cdecl, ty_real, 0)
global.gm2_get_origin_y             = external_define(dll_path, "gm2_get_origin_y", dll_cdecl, ty_real, 0)
global.gm2_get_transform_a00        = external_define(dll_path, "gm2_get_transform_a00", dll_cdecl, ty_real, 0)
global.gm2_get_transform_a01        = external_define(dll_path, "gm2_get_transform_a01", dll_cdecl, ty_real, 0)
global.gm2_get_transform_a10        = external_define(dll_path, "gm2_get_transform_a10", dll_cdecl, ty_real, 0)
global.gm2_get_transform_a11        = external_define(dll_path, "gm2_get_transform_a11", dll_cdecl, ty_real, 0)
global.gm2_get_transform_dx         = external_define(dll_path, "gm2_get_transform_dx", dll_cdecl, ty_real, 0)
global.gm2_get_transform_dy         = external_define(dll_path, "gm2_get_transform_dy", dll_cdecl, ty_real, 0)
global.gm2_font_weight_thin = 100
global.gm2_font_weight_extra_light = 200
global.gm2_font_weight_light = 300
global.gm2_font_weight_normal = 400
global.gm2_font_weight_medium = 500
global.gm2_font_weight_semi_bold = 600
global.gm2_font_weight_bold = 700
global.gm2_font_weight_extra_bold = 800
global.gm2_font_weight_black = 900
global.gm2_font_weight_extra_black = 950
global.gm2_font_style_normal = 0 << 10
global.gm2_font_style_italic = 2 << 10
global.gm2_font_style_oblique = 1 << 10
global.gm2_font_stretch_ultra_condensed = 1 << 12
global.gm2_font_stretch_extra_condensed = 2 << 12
global.gm2_font_stretch_condensed = 3 << 12
global.gm2_font_stretch_semi_condensed = 4 << 12
global.gm2_font_stretch_normal = 5 << 12
global.gm2_font_stretch_semi_expanded = 6 << 12
global.gm2_font_stretch_expanded = 7 << 12
global.gm2_font_stretch_extra_expanded = 8 << 12
global.gm2_font_stretch_ultra_expanded = 9 << 12
global.gm2_text_direction_ltr = 0
global.gm2_text_direction_rtl = 1
global.gm2_par_direction_ttb = 0 << 2
global.gm2_par_direction_btt = 1 << 2
global.gm2_text_alignment_leading = 0
global.gm2_text_alignment_center = 2
global.gm2_text_alignment_trailing = 1
global.gm2_text_alignment_justified = 3
global.gm2_par_alignment_near = 0 << 2
global.gm2_par_alignment_center = 2 << 2
global.gm2_par_alignment_far = 1 << 2
global.gm2_infinity = 100000
global.gm2_word_wrapping_none = 1
global.gm2_word_wrapping_char = 4
global.gm2_word_wrapping_word = 2
global.gm2_word_wrapping_whole_word = 3
global.gm2_trimming_none = 0
global.gm2_trimming_char = 1
global.gm2_trimming_word = 2
global.gm2_line_spacing_type_proportional = 2
global.gm2_line_spacing_type_uniform = 1

external_call(
    global.gm2_internal_new_font,
    external_call(global.gm2_internal_to_real, "default"),
    external_call(global.gm2_internal_to_real, "Source Han Serif SC"),
    global.gm2_font_weight_normal | global.gm2_font_style_normal | global.gm2_font_stretch_normal,
    18,
    external_call(global.gm2_internal_to_real, "")
)
external_call(global.gm2_set_font, "default")
external_call(global.gm2_save_options, "")

page = 0
#define KeyPress_32
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
page = (page + 1) mod 4
#define Draw_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
external_call(global.gm2_draw_text, 0, 0, "FPS: " + string(fps))

var title_height{ title_height = room_height / 2 * .2 }
var table_padding{ table_padding = room_width / 2 * .05 }
var cell_padding{ cell_padding = room_width / 2 * .05 }
if (page == 0) {
    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 2, title_height / 2, "gm2_set_alignment()")

        grid_mesh(
            table_padding,
            title_height,
            3,
            3,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            70,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "leading"
        col_header[2] = "center"
        col_header[3] = "trailing"

        var row_header{}
        row_header[1] = "near"
        row_header[2] = "center"
        row_header[3] = "far"

        var col_param{}
        col_param[1] = global.gm2_text_alignment_leading
        col_param[2] = global.gm2_text_alignment_center
        col_param[3] = global.gm2_text_alignment_trailing

        var row_param{}
        row_param[1] = global.gm2_par_alignment_near
        row_param[2] = global.gm2_par_alignment_center
        row_param[3] = global.gm2_par_alignment_far

        external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[0], "point")
        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], row_header[i])
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                draw_circle_color(grid_mesh_col[col], grid_mesh_row[row], 5, $e6b689, $e6b689, false)
                external_call(global.gm2_set_alignment, col_param[col] | row_param[row])
                external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[row], "Text")
            }
        }

        grid_mesh(
            room_width / 2 + table_padding,
            title_height,
            3,
            3,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            70,
            30,
            $e6b689
        )

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[0], "box")
        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], row_header[i])
        }

        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                external_call(global.gm2_set_alignment, col_param[col] | row_param[row])
                external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[row], "Text")
            }
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 4, room_height / 2 + title_height / 2, "gm2_set_direction()")

        grid_mesh(
            table_padding,
            room_height / 2 + title_height,
            2,
            2,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            70,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "ltr"
        col_header[2] = "rtl"

        var row_header{}
        row_header[1] = "ttb"
        row_header[2] = "btt"

        var col_param{}
        col_param[1] = global.gm2_text_direction_ltr
        col_param[2] = global.gm2_text_direction_rtl

        var row_param{}
        row_param[1] = global.gm2_par_direction_ttb
        row_param[2] = global.gm2_par_direction_btt

        var text{}
        text[1] = "The quick brown fox jumps over the lazy dog."
        text[2] = "يقفز الثعلب البني السريع فوق الكلب الكسول."

        var i{}
        for (i = 1; i <= 2; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], row_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        external_call(global.gm2_set_line_height, .9)
        var row{}
        for (row = 1; row <= 2; row += 1) {
            var col{}
            for (col = 1; col <= 2; col += 1) {
                external_call(global.gm2_set_direction, col_param[col] | row_param[row])
                external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[row], text[col])
            }
        }

        external_call(global.gm2_load_options, "")
    }

    {
        grid_mesh(
            room_width / 2 + table_padding,
            room_height / 2 + title_height,
            1,
            2,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            0,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "leading"
        col_header[2] = "justified"

        var col_param{}
        col_param[1] = global.gm2_text_alignment_leading
        col_param[2] = global.gm2_text_alignment_justified

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        var i{}
        for (i = 1; i <= 2; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var col{}
        for (col = 1; col <= 2; col += 1) {
            external_call(global.gm2_set_text_alignment, col_param[col])
            external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm2_load_options, "")
    }
} else if (page == 1) {
    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 2, title_height / 2, "gm2_set_max_width()")

        grid_mesh(
            table_padding,
            title_height,
            3,
            1,
            room_width - table_padding * 2,
            room_height / 2 - title_height,
            50,
            0,
            $e6b689
        )

        var row_param{}
        row_param[1] = 600
        row_param[2] = 500
        row_param[3] = 400

        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], string(row_param[i]))
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            draw_circle_color(grid_mesh_col[1], grid_mesh_row[row], 5, $e6b689, $e6b689, false)
            external_call(global.gm2_set_max_width, row_param[row])
            external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[row], "It was the best of times, 这是一个最坏的时代, それは智慧の時代であり, 어리석음의 시대이기도 했다.")
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 4, room_height / 2 + title_height / 2, "gm2_set_word_wrapping()")

        grid_mesh(
            table_padding,
            room_height / 2 + title_height,
            1,
            4,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            0,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "none"
        col_header[2] = "char"
        col_header[3] = "word"
        col_header[4] = "whole word"

        var col_param{}
        col_param[1] = global.gm2_word_wrapping_none
        col_param[2] = global.gm2_word_wrapping_char
        col_param[3] = global.gm2_word_wrapping_word
        col_param[4] = global.gm2_word_wrapping_whole_word

        var i{}
        for (i = 1; i <= 4; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        external_call(global.gm2_set_line_height, .9)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            var text{ text = "It is a truth universally acknowledged." }
            if (col != 1) {
                text = string_repeat(chr(10), 2) + text
            }

            external_call(global.gm2_set_word_wrapping, col_param[col])
            external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[1], text)
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width * 3 / 4, room_height / 2 + title_height / 2, "gm2_set_trimming()")

        grid_mesh(
            room_width / 2 + table_padding,
            room_height / 2 + title_height,
            1,
            3,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            0,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "none"
        col_header[2] = "char"
        col_header[3] = "word"

        var col_param{}
        col_param[1] = global.gm2_trimming_none
        col_param[2] = global.gm2_trimming_char
        col_param[3] = global.gm2_trimming_word

        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        external_call(global.gm2_set_word_wrapping, global.gm2_word_wrapping_none)
        var col{}
        for (col = 1; col <= 3; col += 1) {
            var text{ text = "It is a truth universally acknowledged, that a single man in possession of a good fortune, must be in want of a wife." + chr(10) + "However little known the feelings or views of such a man may be on his first entering a neighbourhood, this truth is so well fixed in the minds of the surrounding families, that he is considered the rightful property of some one or other of their daughters." }
            if (col != 1) {
                text = string_repeat(chr(10), 3) + text
            }

            external_call(global.gm2_set_trimming, col_param[col])
            external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[1], text)
        }

        external_call(global.gm2_load_options, "")
    }
} else if (page == 2) {
    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 4, title_height / 2, "gm2_set_line_spacing()")

        grid_mesh(
            table_padding,
            title_height,
            1,
            4,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            0,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "1x, 1x"
        col_header[2] = "0.8x, 1x"
        col_header[3] = "1.2x, 1x"
        col_header[4] = "1x, 1.5x"

        var col_param1{}
        col_param1[1] = 1
        col_param1[2] = .8
        col_param1[3] = 1.2
        col_param1[4] = 1

        var col_param2{}
        col_param2[1] = 1
        col_param2[2] = 1
        col_param2[3] = 1
        col_param2[4] = 1.5

        var i{}
        for (i = 1; i <= 4; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            external_call(global.gm2_set_line_spacing, col_param1[col], col_param2[col])
            external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width * 3 / 4, title_height / 2, "gm2_set_uniform_line_spacing()")

        grid_mesh(
            room_width / 2 + table_padding,
            title_height,
            1,
            4,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            0,
            30,
            $e6b689
        )

        var col_header{}
        col_header[1] = "20, 20"
        col_header[2] = "15, 20"
        col_header[3] = "30, 20"
        col_header[4] = "20, 30"

        var col_param1{}
        col_param1[1] = 20
        col_param1[2] = 15
        col_param1[3] = 30
        col_param1[4] = 20

        var col_param2{}
        col_param2[1] = 20
        col_param2[2] = 20
        col_param2[3] = 20
        col_param2[4] = 30

        var i{}
        for (i = 1; i <= 4; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            external_call(global.gm2_set_uniform_line_spacing, col_param1[col], col_param2[col])
            external_call(global.gm2_draw_text, grid_mesh_col[col], grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width / 4, room_height / 2 + title_height / 2, "gm2_set_tab_spacing()")

        grid_mesh(
            table_padding,
            room_height / 2 + title_height,
            3,
            1,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            50,
            0,
            $e6b689
        )

        var row_param{}
        row_param[1] = 40
        row_param[2] = 80
        row_param[3] = 120

        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], string(row_param[i]))
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            external_call(global.gm2_set_tab_spacing, row_param[row])
            external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[row], chr(9) + "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm2_load_options, "")
    }

    {
        external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
        external_call(global.gm2_draw_text, room_width * 3 / 4, room_height / 2 + title_height / 2, "gm2_set_letter_spacing()")

        grid_mesh(
            room_width / 2 + table_padding,
            room_height / 2 + title_height,
            3,
            1,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            50,
            0,
            $e6b689
        )

        var row_param{}
        row_param[1] = 0
        row_param[2] = 2
        row_param[3] = 5

        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], string(row_param[i]))
        }

        external_call(global.gm2_set_alignment, global.gm2_text_alignment_leading | global.gm2_par_alignment_near)
        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        external_call(global.gm2_set_origin, .5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            external_call(global.gm2_set_letter_spacing, row_param[row])
            external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[row], "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm2_load_options, "")
    }
} else if (page == 3) {
    external_call(global.gm2_set_alignment, global.gm2_text_alignment_center | global.gm2_par_alignment_center)
    external_call(global.gm2_draw_text, room_width / 2, title_height / 2, "gm2_set_transform()")

    grid_mesh(
        table_padding,
        title_height,
        5,
        1,
        room_width - table_padding * 2,
        room_height - title_height - table_padding,
        120,
        0,
        $e6b689
    )

    var row_header{}
    row_header[1] = "translation"
    row_header[2] = "scale"
    row_header[3] = "rotation"
    row_header[4] = "skew"
    row_header[5] = "combined"

    var i{}
    for (i = 1; i <= 5; i += 1) {
        external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], row_header[i])
    }

    external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
    external_call(global.gm2_set_origin, .5, .5)

    external_call(global.gm2_set_transform, 1, 0, 0, 1, 0, 0)
    external_call(global.gm2_add_translation, sin(current_time / 500) * 20, sin(current_time / 700) * 10)
    external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[1], "Lorem ipsum dolor sit amet.")

    external_call(global.gm2_set_transform, 1, 0, 0, 1, 0, 0)
    external_call(global.gm2_add_scale, 1 + sin(current_time / 600) * .5, 1 + sin(current_time / 750) * .5)
    external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[2], "Lorem ipsum dolor sit amet.")

    external_call(global.gm2_set_transform, 1, 0, 0, 1, 0, 0)
    external_call(global.gm2_add_rotation, sin(current_time / 800) * 10)
    external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[3], "Lorem ipsum dolor sit amet.")

    external_call(global.gm2_set_transform, 1, 0, 0, 1, 0, 0)
    external_call(global.gm2_add_skew, sin(current_time / 500) * 20, sin(current_time / 700) * 10)
    external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[4], "Lorem ipsum dolor sit amet.")

    var phase{ phase = current_time / 400 }
    external_call(global.gm2_set_transform, 1, 0, 0, 1, 0, 0)
    external_call(global.gm2_add_rotation, sin(phase) * 12)
    external_call(global.gm2_add_scale, 1.15, 1.15)
    external_call(global.gm2_add_skew_x, cos(phase) * 12)
    external_call(global.gm2_add_translation, cos(phase) * 60, sin(phase) * 10)
    external_call(global.gm2_draw_text, grid_mesh_col[1], grid_mesh_row[5], "Lorem ipsum dolor sit amet.")

    external_call(global.gm2_load_options, "")
}
