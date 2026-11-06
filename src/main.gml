#define gm2_new_font
var arg3{ arg3 = gm2_font_weight_normal | gm2_font_style_normal | gm2_font_stretch_normal }
var arg4{ arg4 = "" }
if (argument_count >= 4) {
    arg3 = argument[3]

    if (argument_count >= 5) {
        arg4 = argument[4]
    }
}

return gm2_internal_new_font(
    gm2_internal_to_real(argument0),
    gm2_internal_to_real(argument1),
    argument2,
    arg3,
    gm2_internal_to_real(arg4)
)

#define gm2_make_rgb
var arg0{}
var arg1{}
var arg2{}
var arg3{ arg3 = 1 }
if (argument_count >= 3) {
    arg0 = argument0
    arg1 = argument[1]
    arg2 = argument[2]

    if (argument_count >= 4) {
        arg3 = argument[3]
    }
} else {
    arg0 = argument0 & 255
    arg1 = argument0 >> 8 & 255
    arg2 = argument0 >> 16 & 255

    if (argument_count >= 2) {
        arg3 = argument[1]
    }
}

return gm2_internal_make_rgb(arg0, arg1, arg2, arg3)

#define gm2_make_hsv
var arg3{ arg3 = 1 }
if (argument_count >= 4) {
    arg3 = argument[3]
}

return gm2_internal_make_hsv(argument0, argument1, argument2, arg3)

#define gm2_set_transform
var arg0{ arg0 = 1 }
var arg1{ arg1 = 0 }
var arg2{ arg2 = 0 }
var arg3{ arg3 = 1 }
var arg4{ arg4 = 0 }
var arg5{ arg5 = 0 }
if (argument_count >= 4) {
    arg0 = argument[0]
    arg1 = argument[1]
    arg2 = argument[2]
    arg3 = argument[3]

    if (argument_count >= 6) {
        arg4 = argument[4]
        arg5 = argument[5]
    }
}

return gm2_internal_set_transform(arg0, arg1, arg2, arg3, arg4, arg5)

#define gm2_add_transform
var arg4{ arg4 = 0 }
var arg5{ arg5 = 0 }
if (argument_count >= 6) {
    arg4 = argument[4]
    arg5 = argument[5]
}

return gm2_internal_add_transform(argument0, argument1, argument2, argument3, arg4, arg5)

#define gm2_set_color
var arg1{}
var arg2{}
var arg3{}
if (argument_count >= 4) {
    arg1 = argument[1]
    arg2 = argument[2]
    arg3 = argument[3]
} else if (argument_count >= 2) {
    arg1 = argument0
    arg2 = argument[1]
    arg3 = argument[1]
} else {
    arg1 = argument0
    arg2 = argument0
    arg3 = argument0
}

return gm2_internal_set_color(argument0, arg1, arg2, arg3)
