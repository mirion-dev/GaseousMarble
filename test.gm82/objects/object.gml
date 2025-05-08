#define Create_0
/*"/*'/**//* YYD ACTION
lib_id=1
action_id=603
applies_to=self
*/
var dll_path
if (parameter_string(1) == '-debug') {
   dll_path = '../Debug/GaseousMarble.dll'
}
else {
   dll_path = '../Release/GaseousMarble.dll'
}

global.gm_init = external_define(dll_path, 'gm_init', dll_cdecl, ty_real, 0)
global.gm_font = external_define(dll_path, 'gm_font', dll_cdecl, ty_real, 2, ty_string, ty_string)
global.gm_draw = external_define(dll_path, 'gm_draw', dll_cdecl, ty_real, 3, ty_real, ty_real, ty_string)
global.gm_set_font = external_define(dll_path, 'gm_set_font', dll_cdecl, ty_real, 1, ty_string)
global.gm_set_line_height = external_define(dll_path, 'gm_set_line_height', dll_cdecl, ty_real, 1, ty_real)
global.gm_set_max_line_length = external_define(dll_path, 'gm_set_max_line_length', dll_cdecl, ty_real, 1, ty_real)

external_call(global.gm_init)
external_call(global.gm_font, 'default', './resources/font_default.png')
external_call(global.gm_set_font, 'default')
external_call(global.gm_set_max_line_length, room_width)
external_call(global.gm_set_line_height, .86)
draw_set_color(c_white)
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
draw_text(0, 0, fps)

external_call(global.gm_draw, 0, 0, "
在游戏中你需要绘制文本。要绘制文本你需要先指定要使用的字体。字体可以通过字体资源创建（不管是在 GM 设计界面里还是使用函数创建资源）。这里有很多函数可以通过不同方法绘制文本。每个函数你都要指定文本在屏幕上显示的位置。有两个函数负责指定文本的水平及垂直坐标。

文本的绘制涉及以下函数：

draw_set_font(font) 设定绘制文本时将要使用的字体。-1 代表默认字体（Arial 12）。

draw_set_halign(halign) 设定绘制文本的水平坐标参数。选择下面三个中的一个作为值：
fa_left 左
fa_center 中
fa_right 右

draw_set_valign(valign) 设定绘制文本的垂直坐标参数。选择下面三个中的一个作为值：
fa_top 上
fa_middle 中
fa_bottom 下

draw_text(x, y, string) 在坐标 (x, y) 处绘制字符串 string，一个 '#' 通配符或者一个回车符 chr(13) 或者断行符 chr(10) 会让字符串另起一行，这样我们就可以实现多行文本的绘制（使用 '\\#' 显示字符 '#' 本身）。
draw_text_ext(x, y, string, sep, w) 基本与上面的函数作用相同，但增加了两个功能。首先 sep 代表行间距，设成 -1 代表使用默认值。w 代表行宽，单位像素。超出行宽的部分会以空格或 '-' 进行分行。设为 -1 代表不换行。
string_width(string) 当前字体及将要通过 draw_text() 函数绘制的字符串 string 的宽度。可以用来精确定位图像位置。
string_height(string) 当前字体及将要通过 draw_text() 函数绘制的字符串 string 的高度。可以用来精确定位图像位置。
string_width_ext(string, sep, w) 当前字体及将要通过 draw_text_ext() 函数绘制的字符串 string 的宽度。可以用来精确定位图像位置。sep 代表行间距，w 代表行宽。
string_height_ext(string, sep, w) 当前字体及将要通过 draw_text_ext() 函数绘制的字符串 string 的高度。可以用来精确定位图像位置。sep 代表行间距，w 代表行宽。")
