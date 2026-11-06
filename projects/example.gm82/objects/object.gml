#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
gm2_new_font("default", "Source Han Serif SC", 18)
gm2_set_font("default")
gm2_save_options("")

page = 0
#define Draw_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
gm2_draw_text(0, 0, "FPS: " + string(fps))

var title_height{ title_height = room_height / 2 * .2 }
var table_padding{ table_padding = room_width / 2 * .05 }
var cell_padding{ cell_padding = room_width / 2 * .05 }
if (page == 0) {
    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 2, title_height / 2, "gm2_set_alignment()")

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
        col_param[1] = gm2_text_alignment_leading
        col_param[2] = gm2_text_alignment_center
        col_param[3] = gm2_text_alignment_trailing

        var row_param{}
        row_param[1] = gm2_par_alignment_near
        row_param[2] = gm2_par_alignment_center
        row_param[3] = gm2_par_alignment_far

        gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[0], "point")
        var i{}
        for (i = 1; i <= 3; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], row_header[i])
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                draw_circle_color(global.grid_mesh_col[col], global.grid_mesh_row[row], 5, $e6b689, $e6b689, false)
                gm2_set_alignment(col_param[col] | row_param[row])
                gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[row], "Text")
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

        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[0], "box")
        var i{}
        for (i = 1; i <= 3; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], row_header[i])
        }

        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            var col{}
            for (col = 1; col <= 3; col += 1) {
                gm2_set_alignment(col_param[col] | row_param[row])
                gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[row], "Text")
            }
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 4, room_height / 2 + title_height / 2, "gm2_set_direction()")

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
        col_param[1] = gm2_text_direction_ltr
        col_param[2] = gm2_text_direction_rtl

        var row_param{}
        row_param[1] = gm2_par_direction_ttb
        row_param[2] = gm2_par_direction_btt

        var text{}
        text[1] = "The quick brown fox jumps over the lazy dog."
        text[2] = "يقفز الثعلب البني السريع فوق الكلب الكسول."

        var i{}
        for (i = 1; i <= 2; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], row_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        gm2_set_line_height(.9)
        var row{}
        for (row = 1; row <= 2; row += 1) {
            var col{}
            for (col = 1; col <= 2; col += 1) {
                gm2_set_direction(col_param[col] | row_param[row])
                gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[row], text[col])
            }
        }

        gm2_load_options("")
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
        col_param[1] = gm2_text_alignment_leading
        col_param[2] = gm2_text_alignment_justified

        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        var i{}
        for (i = 1; i <= 2; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var col{}
        for (col = 1; col <= 2; col += 1) {
            gm2_set_text_alignment(col_param[col])
            gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        gm2_load_options("")
    }
} else if (page == 1) {
    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 2, title_height / 2, "gm2_set_max_width()")

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
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], string(row_param[i]))
        }

        var row{}
        for (row = 1; row <= 3; row += 1) {
            draw_circle_color(global.grid_mesh_col[1], global.grid_mesh_row[row], 5, $e6b689, $e6b689, false)
            gm2_set_max_width(row_param[row])
            gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[row], "It was the best of times, 这是一个最坏的时代, それは智慧の時代であり, 어리석음의 시대이기도 했다.")
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 4, room_height / 2 + title_height / 2, "gm2_set_word_wrapping()")

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
        col_param[1] = gm2_word_wrapping_none
        col_param[2] = gm2_word_wrapping_char
        col_param[3] = gm2_word_wrapping_word
        col_param[4] = gm2_word_wrapping_whole_word

        var i{}
        for (i = 1; i <= 4; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        gm2_set_line_height(.9)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            var text{ text = "It is a truth universally acknowledged." }
            if (col != 1) {
                text = string_repeat(chr(10), 2) + text
            }

            gm2_set_word_wrapping(col_param[col])
            gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[1], text)
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width * 3 / 4, room_height / 2 + title_height / 2, "gm2_set_trimming()")

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
        col_param[1] = gm2_trimming_none
        col_param[2] = gm2_trimming_char
        col_param[3] = gm2_trimming_word

        var i{}
        for (i = 1; i <= 3; i += 1) {
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        gm2_set_word_wrapping(gm2_word_wrapping_none)
        var col{}
        for (col = 1; col <= 3; col += 1) {
            var text{ text = "It is a truth universally acknowledged, that a single man in possession of a good fortune, must be in want of a wife." + chr(10) + "However little known the feelings or views of such a man may be on his first entering a neighbourhood, this truth is so well fixed in the minds of the surrounding families, that he is considered the rightful property of some one or other of their daughters." }
            if (col != 1) {
                text = string_repeat(chr(10), 3) + text
            }

            gm2_set_trimming(col_param[col])
            gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[1], text)
        }

        gm2_load_options("")
    }
} else if (page == 2) {
    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 4, title_height / 2, "gm2_set_line_spacing()")

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
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            gm2_set_line_spacing(col_param1[col], col_param2[col])
            gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width * 3 / 4, title_height / 2, "gm2_set_uniform_line_spacing()")

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
            gm2_draw_text(global.grid_mesh_col[i], global.grid_mesh_row[0], col_header[i])
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var col{}
        for (col = 1; col <= 4; col += 1) {
            gm2_set_uniform_line_spacing(col_param1[col], col_param2[col])
            gm2_draw_text(global.grid_mesh_col[col], global.grid_mesh_row[1], "The quick brown fox jumps over the lazy dog.")
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width / 4, room_height / 2 + title_height / 2, "gm2_set_tab_spacing()")

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
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], string(row_param[i]))
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            gm2_set_tab_spacing(row_param[row])
            gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[row], chr(9) + "The quick brown fox jumps over the lazy dog.")
        }

        gm2_load_options("")
    }

    {
        gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
        gm2_draw_text(room_width * 3 / 4, room_height / 2 + title_height / 2, "gm2_set_letter_spacing()")

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
            gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], string(row_param[i]))
        }

        gm2_set_alignment(gm2_text_alignment_leading | gm2_par_alignment_near)
        gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
        gm2_set_origin(.5, .5)
        var row{}
        for (row = 1; row <= 3; row += 1) {
            gm2_set_letter_spacing(row_param[row])
            gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[row], "The quick brown fox jumps over the lazy dog.")
        }

        gm2_load_options("")
    }
} else if (page == 3) {
    gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
    gm2_draw_text(room_width / 2, title_height / 2, "gm2_set_transform()")

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
        gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], row_header[i])
    }

    gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
    gm2_set_origin(.5, .5)

    gm2_set_transform()
    gm2_add_translation(sin(current_time / 500) * 20, sin(current_time / 700) * 10)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[1], "Lorem ipsum dolor sit amet.")

    gm2_set_transform()
    gm2_add_scale(1 + sin(current_time / 600) * .5, 1 + sin(current_time / 750) * .5)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[2], "Lorem ipsum dolor sit amet.")

    gm2_set_transform()
    gm2_add_rotation(sin(current_time / 800) * pi / 18)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[3], "Lorem ipsum dolor sit amet.")

    gm2_set_transform()
    gm2_add_skew(sin(current_time / 500) * pi / 9, sin(current_time / 700) * pi / 18)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[4], "Lorem ipsum dolor sit amet.")

    var phase{ phase = current_time / 400 }
    gm2_set_transform()
    gm2_add_rotation(sin(phase) * pi / 15)
    gm2_add_scale(1.15, 1.15)
    gm2_add_skew_x(cos(phase) * pi / 15)
    gm2_add_translation(cos(phase) * 60, sin(phase) * 10)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[5], "Lorem ipsum dolor sit amet.")

    gm2_load_options("")
} else if (page == 4) {
    grid_mesh(
        table_padding,
        table_padding,
        5,
        1,
        room_width - table_padding * 2,
        room_height - table_padding * 2,
        170,
        0,
        $e6b689
    )

    var row_header{}
    row_header[1] = "gm2_set_color()"
    row_header[2] = "gm2_set_opacity()"
    row_header[3] = ""
    row_header[4] = ""
    row_header[5] = ""

    gm2_set_alignment(gm2_text_alignment_center | gm2_par_alignment_center)
    var i{}
    for (i = 1; i <= 5; i += 1) {
        gm2_draw_text(global.grid_mesh_col[0], global.grid_mesh_row[i], row_header[i])
    }

    gm2_set_max_size(global.grid_mesh_cell_width, global.grid_mesh_cell_height)
    gm2_set_origin(.5, .5)
    gm2_save_options("item")

    var hue{ hue = current_time / 8 mod 360 }
    gm2_set_color(
        gm2_make_hsv(hue, 1, 1),
        gm2_make_hsv(hue - 90, 1, 1),
        gm2_make_hsv(hue - 180, 1, 1),
        gm2_make_hsv(hue - 270, 1, 1)
    )
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[1], "Lorem ipsum dolor sit amet.")
    gm2_load_options("item")

    gm2_set_opacity((1 + sin(current_time / 700)) / 2)
    gm2_draw_text(global.grid_mesh_col[1], global.grid_mesh_row[2], "Lorem ipsum dolor sit amet.")
    gm2_load_options("item")

    gm2_load_options("")
}
#define KeyPress_32
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
page = (page + 1) mod 5
