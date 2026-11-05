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

external_call(global.gm2_internal_set_transform, arg0, arg1, arg2, arg3, arg4, arg5)
