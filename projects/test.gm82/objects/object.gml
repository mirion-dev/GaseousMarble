#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dll_path{ dll_path = parameter_string(1) }
global.gm_internal_string_to_real = external_define(dll_path, "gm_internal_string_to_real", dll_cdecl, ty_real, 1, ty_string)
global.gm_internal_new_font = external_define(dll_path, "gm_internal_new_font", dll_cdecl, ty_real, 7, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real, ty_real)
global.gm_set_font = external_define(dll_path, "gm_set_font", dll_cdecl, ty_real, 1, ty_string)
global.gm_draw = external_define(dll_path, "gm_draw", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)

error = external_call(
    global.gm_internal_new_font,
    external_call(global.gm_internal_string_to_real, "default"),
    external_call(global.gm_internal_string_to_real, "Microsoft YaHei"),
    100,
    400,
    0,
    5,
    external_call(global.gm_internal_string_to_real, "")
)
if (error < 0) {
    show_error("gm_internal_new_font error code: " + string(error), true)
}

error = external_call(global.gm_set_font, "default")
if (error < 0) {
    show_error("gm_set_font error code: " + string(error), true)
}
#define Keyboard_82
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
game_restart()
#define Draw_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
draw_set_color(c_white)
draw_text(0, 0, fps)

var error{ error = external_call(global.gm_draw, 0, 0, "First") }
if (error < 0) {
    show_error("gm_draw error code: " + string(error), true)
}

error = external_call(global.gm_draw, 0, 80, "Second")
if (error < 0) {
    show_error("gm_draw error code: " + string(error), true)
}

error = external_call(global.gm_draw, 0, 160, "Third")
if (error < 0) {
    show_error("gm_draw error code: " + string(error), true)
}

error = external_call(global.gm_draw, 0, 240, "Fourth")
if (error < 0) {
    show_error("gm_draw error code: " + string(error), true)
}

error = external_call(global.gm_draw, 0, 320, "Fifth")
if (error < 0) {
    show_error("gm_draw error code: " + string(error), true)
}
