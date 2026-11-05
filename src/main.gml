#define gm2_new_font
var arg3{}
if (argument_count >= 4) {
    arg3 = argument[3]
} else {
    arg3 = gm2_font_weight_normal | gm2_font_style_normal | gm2_font_stretch_normal
}

var arg4{}
if (argument_count >= 5) {
    arg4 = argument[4]
} else {
    arg4 = ""
}

gm2_internal_new_font(
    gm2_internal_to_real(argument0),
    gm2_internal_to_real(argument1),
    argument2,
    arg3,
    gm2_internal_to_real(arg4)
)
