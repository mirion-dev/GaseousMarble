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
global.gm2_text_width               = external_define(dll_path, "gm2_text_width", dll_cdecl, ty_real, 1, ty_string)
global.gm2_text_height              = external_define(dll_path, "gm2_text_height", dll_cdecl, ty_real, 1, ty_string)
global.gm2_push_options             = external_define(dll_path, "gm2_push_options", dll_cdecl, ty_real, 0)
global.gm2_pop_options              = external_define(dll_path, "gm2_pop_options", dll_cdecl, ty_real, 0)
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
global.gm2_word_wrapping_none = 0
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

page = 0
#define KeyPress_32
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
page = 1 - page
#define Draw_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var title_height{ title_height = room_height / 2 * .2 }
var table_padding{ table_padding = room_width / 2 * .05 }
var cell_padding{ cell_padding = room_width / 2 * .05 }
if (page == 0) {
    {
        external_call(global.gm2_push_options)
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
        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm2_draw_text, grid_mesh_col[i], grid_mesh_row[0], col_header[i])
            external_call(global.gm2_draw_text, grid_mesh_col[0], grid_mesh_row[i], row_header[i])
        }

        external_call(global.gm2_set_max_size, grid_mesh_cell_width, grid_mesh_cell_height)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                external_call(global.gm2_set_alignment, col_param[col] | row_param[row])
                external_call(global.gm2_draw_text, grid_mesh_col[col] - grid_mesh_cell_width / 2, grid_mesh_row[row] - grid_mesh_cell_height / 2, "Text")
            }
        }

        external_call(global.gm2_pop_options)
    }
}
