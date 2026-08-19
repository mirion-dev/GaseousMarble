#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dll_path{ dll_path = parameter_string(1) }
global.gm_font                  = external_define(dll_path, "gm_font", dll_cdecl, ty_real, 2, ty_string, ty_string)
global.gm_free                  = external_define(dll_path, "gm_free", dll_cdecl, ty_real, 1, ty_string)
global.gm_clear                 = external_define(dll_path, "gm_clear", dll_cdecl, ty_real, 0)
global.gm_draw                  = external_define(dll_path, "gm_draw", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)
global.gm_width                 = external_define(dll_path, "gm_width", dll_cdecl, ty_real, 1, ty_string)
global.gm_height                = external_define(dll_path, "gm_height", dll_cdecl, ty_real, 1, ty_string)
global.gm_set_font              = external_define(dll_path, "gm_set_font", dll_cdecl, ty_real, 1, ty_string)
global.gm_set_halign            = external_define(dll_path, "gm_set_halign", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_valign            = external_define(dll_path, "gm_set_valign", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_justified         = external_define(dll_path, "gm_set_justified", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_align             = external_define(dll_path, "gm_set_align", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm_set_align3            = external_define(dll_path, "gm_set_align3", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_real)
global.gm_set_color             = external_define(dll_path, "gm_set_color", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_color2            = external_define(dll_path, "gm_set_color2", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm_set_alpha             = external_define(dll_path, "gm_set_alpha", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_letter_spacing    = external_define(dll_path, "gm_set_letter_spacing", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_word_spacing      = external_define(dll_path, "gm_set_word_spacing", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_paragraph_spacing = external_define(dll_path, "gm_set_paragraph_spacing", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_line_height       = external_define(dll_path, "gm_set_line_height", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_max_line_length   = external_define(dll_path, "gm_set_max_line_length", dll_cdecl, ty_real, 1, ty_real)
global.gm_set_offset            = external_define(dll_path, "gm_set_offset", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm_set_scale             = external_define(dll_path, "gm_set_scale", dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm_set_rotation          = external_define(dll_path, "gm_set_rotation", dll_cdecl, ty_real, 1, ty_real)
global.gm_get_font              = external_define(dll_path, "gm_get_font", dll_cdecl, ty_string, 0)
global.gm_get_halign            = external_define(dll_path, "gm_get_halign", dll_cdecl, ty_real, 0)
global.gm_get_valign            = external_define(dll_path, "gm_get_valign", dll_cdecl, ty_real, 0)
global.gm_is_justified          = external_define(dll_path, "gm_is_justified", dll_cdecl, ty_real, 0)
global.gm_get_color_top         = external_define(dll_path, "gm_get_color_top", dll_cdecl, ty_real, 0)
global.gm_get_color_bottom      = external_define(dll_path, "gm_get_color_bottom", dll_cdecl, ty_real, 0)
global.gm_get_alpha             = external_define(dll_path, "gm_get_alpha", dll_cdecl, ty_real, 0)
global.gm_get_letter_spacing    = external_define(dll_path, "gm_get_letter_spacing", dll_cdecl, ty_real, 0)
global.gm_get_word_spacing      = external_define(dll_path, "gm_get_word_spacing", dll_cdecl, ty_real, 0)
global.gm_get_paragraph_spacing = external_define(dll_path, "gm_get_paragraph_spacing", dll_cdecl, ty_real, 0)
global.gm_get_line_height       = external_define(dll_path, "gm_get_line_height", dll_cdecl, ty_real, 0)
global.gm_get_max_line_length   = external_define(dll_path, "gm_get_max_line_length", dll_cdecl, ty_real, 0)
global.gm_get_offset_x          = external_define(dll_path, "gm_get_offset_x", dll_cdecl, ty_real, 0)
global.gm_get_offset_y          = external_define(dll_path, "gm_get_offset_y", dll_cdecl, ty_real, 0)
global.gm_get_scale_x           = external_define(dll_path, "gm_get_scale_x", dll_cdecl, ty_real, 0)
global.gm_get_scale_y           = external_define(dll_path, "gm_get_scale_y", dll_cdecl, ty_real, 0)
global.gm_get_rotation          = external_define(dll_path, "gm_get_rotation", dll_cdecl, ty_real, 0)

external_call(global.gm_font, "default", "gm_fonts/default.png")
external_call(global.gm_set_font, "default")

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
        external_call(global.gm_set_align, 0, 0)
        external_call(global.gm_draw, room_width / 4, title_height / 2, "gm_set_align()")

        grid_mesh(
            table_padding,
            title_height,
            3,
            3,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            30,
            30,
            $e6b689
        )

        var i{}
        for (i = 1; i <= 3; i += 1) {
            external_call(global.gm_draw, grid_mesh_col[i], grid_mesh_row[0], string(i - 2))
            external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[i], string(i - 2))
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                draw_circle_color(grid_mesh_col[col], grid_mesh_row[row], 5, $e6b689, $e6b689, false)
                external_call(global.gm_set_align, col - 2, row - 2)
                external_call(global.gm_draw, grid_mesh_col[col], grid_mesh_row[row], "Text")
            }
        }

        external_call(global.gm_set_align, -1, -1)
    }

    {
        external_call(global.gm_set_align, 0, 0)
        external_call(global.gm_draw, room_width * 3 / 4, title_height / 2, "gm_set_justified()")

        grid_mesh(
            room_width / 2 + table_padding,
            title_height,
            1,
            2,
            room_width / 2 - table_padding * 2,
            room_height / 2 - title_height,
            0,
            30,
            $e6b689
        )

        external_call(global.gm_set_max_line_length, grid_mesh_cell_width - cell_padding * 2)

        header[0] = "false"
        header[1] = "true"

        var col{}
        for (col = 1; col <= 2; col += 1) {
            external_call(global.gm_set_justified, col - 1)
            external_call(global.gm_draw, grid_mesh_col[col], grid_mesh_row[0], header[col - 1])
            external_call(global.gm_draw, grid_mesh_col[col], grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        external_call(global.gm_set_align3, -1, -1, false)
        external_call(global.gm_set_max_line_length, 0)
    }

    {
        external_call(global.gm_set_align, 0, 0)
        external_call(global.gm_draw, room_width / 2, room_height / 2 + title_height / 2, "gm_set_max_line_length()")

        grid_mesh(
            table_padding,
            room_height / 2 + title_height,
            3,
            1,
            room_width - table_padding * 2,
            room_height / 2 - title_height - table_padding,
            50,
            0,
            $e6b689
        )

        external_call(global.gm_set_justified, true)

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var max_length{ max_length = 600 - (row - 1) * 100 }
            external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[row], string(max_length))
            external_call(global.gm_set_max_line_length, max_length)
            external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[row], "It was the best of times, 这是一个最坏的时代, それは智慧の時代であり, 어리석음의 시대이기도 했다.")
        }

        external_call(global.gm_set_align3, -1, -1, false)
        external_call(global.gm_set_max_line_length, 0)
    }
} else {
    grid_mesh(
        table_padding,
        table_padding,
        5,
        1,
        room_width - table_padding * 2,
        room_height - table_padding * 2,
        180,
        0,
        $e6b689
    )

    external_call(global.gm_set_align, 0, 0)
    external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[1], "gm_set_color2()")
    external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[2], "gm_set_alpha()")
    external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[3], "gm_set_offset()")
    external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[4], "gm_set_scale()")
    external_call(global.gm_draw, grid_mesh_col[0], grid_mesh_row[5], "gm_set_rotation()")

    external_call(global.gm_set_color2, make_color_hsv((1 + sin(current_time / 900)) / 2 * 255, 255, 255), make_color_hsv((1 + sin(current_time / 900 + 2)) / 2 * 255, 255, 255))
    external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[1], "Lorem ipsum dolor sit amet.")
    external_call(global.gm_set_color, c_white)

    external_call(global.gm_set_alpha, (1 + sin(current_time / 700)) / 2)
    external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[2], "Lorem ipsum dolor sit amet.")
    external_call(global.gm_set_alpha, 1)

    external_call(global.gm_set_offset, sin(current_time / 500) * 20, sin(current_time / 700) * 10)
    external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[3], "Lorem ipsum dolor sit amet.")
    external_call(global.gm_set_offset, 0, 0)

    external_call(global.gm_set_scale, 1 + sin(current_time / 600) * .5, 1 + sin(current_time / 750) * .5)
    external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[4], "Lorem ipsum dolor sit amet.")
    external_call(global.gm_set_scale, 1, 1)

    external_call(global.gm_set_rotation, sin(current_time / 800) * 10)
    external_call(global.gm_draw, grid_mesh_col[1], grid_mesh_row[5], "Lorem ipsum dolor sit amet.")
    external_call(global.gm_set_rotation, 0)

    external_call(global.gm_set_align, -1, -1)
}
