#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dll_path{ dll_path = parameter_string(1) }
global.gm_init = external_define(dll_path, "gm_init", dll_cdecl, ty_real, 0)
global.gm_draw = external_define(dll_path, "gm_draw", dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)

var error{ error = external_call(global.gm_init) }
if (error < 0) {
    show_error("gm_init error code: " + string(error), true)
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
