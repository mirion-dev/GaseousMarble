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

return external_call(global.gm2_internal_make_rgb, arg0, arg1, arg2, arg3)
