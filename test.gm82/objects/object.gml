#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dllpath { dllpath = '../Debug/GaseousMarble.dll' }
global.gm_font = external_define(dllpath, 'gm_font', dll_cdecl, ty_real, 2, ty_string, ty_string)
global.gm_draw = external_define(dllpath, 'gm_draw', dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)
global.gm_clear = external_define(dllpath, 'gm_clear', dll_cdecl, ty_real, 0)
global.gm_set_font = external_define(dllpath, 'gm_set_font', dll_cdecl, ty_real, 1, ty_real)
global.gm_set_color2 = external_define(dllpath, 'gm_set_color2', dll_cdecl, ty_real, 2, ty_real, ty_real)
global.gm_set_align = external_define(dllpath, 'gm_set_align', dll_cdecl, ty_real, 2, ty_real, ty_real)

global.font_default = external_call(global.gm_font, './plugins/font_default.png', './plugins/font_default.gly')
external_call(global.gm_set_font, global.font_default)
#define Keyboard_82
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
external_call(global.gm_clear)
game_restart()
#define Draw_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
external_call(global.gm_set_color2, c_yellow, c_orange)
external_call(global.gm_set_align, 0, 0)
external_call(global.gm_draw, room_width / 2, room_height / 2, 'To be or not to be, that is the question
生存还是毁灭，这是一个值得考虑的问题
生存還是毀滅，這是一個值得考慮的問題')
