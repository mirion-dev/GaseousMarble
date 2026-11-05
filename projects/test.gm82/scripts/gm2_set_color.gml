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

external_call(global.gm2_internal_set_color, argument0, arg1, arg2, arg3)
