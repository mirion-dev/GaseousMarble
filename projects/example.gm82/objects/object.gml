#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
gm_font("default", "gm_fonts/default.png")
gm_set_font("default")

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
        gm_set_align(0, 0)
        gm_draw(room_width / 4, title_height / 2, "gm_set_align()")

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
            gm_draw(grid_mesh_col[i], grid_mesh_row[0], string(i - 2))
            gm_draw(grid_mesh_col[0], grid_mesh_row[i], string(i - 2))
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                draw_circle_color(grid_mesh_col[col], grid_mesh_row[row], 5, $e6b689, $e6b689, false)
                gm_set_align(col - 2, row - 2)
                gm_draw(grid_mesh_col[col], grid_mesh_row[row], "Text")
            }
        }

        gm_set_align(-1, -1)
    }

    {
        gm_set_align(0, 0)
        gm_draw(room_width * 3 / 4, title_height / 2, "gm_set_justified()")

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

        gm_set_max_line_length(grid_mesh_cell_width - cell_padding * 2)

        header[0] = "false"
        header[1] = "true"

        var col{}
        for (col = 1; col <= 2; col += 1) {
            gm_set_justified(col - 1)
            gm_draw(grid_mesh_col[col], grid_mesh_row[0], header[col - 1])
            gm_draw(grid_mesh_col[col], grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        gm_set_align3(-1, -1, false)
        gm_set_max_line_length(0)
    }

    {
        gm_set_align(0, 0)
        gm_draw(room_width / 2, room_height / 2 + title_height / 2, "gm_set_max_line_length()")

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

        gm_set_justified(true)

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var max_length{ max_length = 600 - (row - 1) * 100 }
            gm_draw(grid_mesh_col[0], grid_mesh_row[row], string(max_length))
            gm_set_max_line_length(max_length)
            gm_draw(grid_mesh_col[1], grid_mesh_row[row], "It was the best of times, 这是一个最坏的时代, それは智慧の時代であり, 어리석음의 시대이기도 했다.")
        }

        gm_set_align3(-1, -1, false)
        gm_set_max_line_length(0)
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

    gm_set_align(0, 0)
    gm_draw(grid_mesh_col[0], grid_mesh_row[1], "gm_set_color2()")
    gm_draw(grid_mesh_col[0], grid_mesh_row[2], "gm_set_alpha()")
    gm_draw(grid_mesh_col[0], grid_mesh_row[3], "gm_set_offset()")
    gm_draw(grid_mesh_col[0], grid_mesh_row[4], "gm_set_scale()")
    gm_draw(grid_mesh_col[0], grid_mesh_row[5], "gm_set_rotation()")

    gm_set_color2(make_color_hsv((1 + sin(current_time / 900)) / 2 * 255, 255, 255), make_color_hsv((1 + sin(current_time / 900 + 2)) / 2 * 255, 255, 255))
    gm_draw(grid_mesh_col[1], grid_mesh_row[1], "Lorem ipsum dolor sit amet.")
    gm_set_color(c_white)

    gm_set_alpha((1 + sin(current_time / 700)) / 2)
    gm_draw(grid_mesh_col[1], grid_mesh_row[2], "Lorem ipsum dolor sit amet.")
    gm_set_alpha(1)

    gm_set_offset(sin(current_time / 500) * 20, sin(current_time / 700) * 10)
    gm_draw(grid_mesh_col[1], grid_mesh_row[3], "Lorem ipsum dolor sit amet.")
    gm_set_offset(0, 0)

    gm_set_scale(1 + sin(current_time / 600) * .5, 1 + sin(current_time / 750) * .5)
    gm_draw(grid_mesh_col[1], grid_mesh_row[4], "Lorem ipsum dolor sit amet.")
    gm_set_scale(1, 1)

    gm_set_rotation(sin(current_time / 800) * 10)
    gm_draw(grid_mesh_col[1], grid_mesh_row[5], "Lorem ipsum dolor sit amet.")
    gm_set_rotation(0)

    gm_set_align(-1, -1)
}
