var arg3{ arg3 = gm2_font_weight_normal | gm2_font_style_normal | gm2_font_stretch_normal }
var arg4{ arg4 = "" }
if (argument_count >= 4) {
    arg3 = argument[3]

    if (argument_count >= 5) {
        arg4 = argument[4]
    }
}

return external_call(global.gm2_internal_new_font,
    external_call(global.gm2_internal_to_real, argument0),
    external_call(global.gm2_internal_to_real, argument1),
    argument2,
    arg3,
    external_call(global.gm2_internal_to_real, arg4)
)
