if (argument_count == 3) {
    return external_call(global.gm2_internal_make_hsv, argument0, argument1, argument2, 1)
}

return external_call(global.gm2_internal_make_hsv, argument0, argument1, argument2, argument[3])
