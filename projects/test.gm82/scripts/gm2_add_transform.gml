var arg4{ arg4 = 0 }
var arg5{ arg5 = 0 }
if (argument_count >= 6) {
    arg4 = argument[4]
    arg5 = argument[5]
}

return external_call(global.gm2_internal_add_transform, argument0, argument1, argument2, argument3, arg4, arg5)
