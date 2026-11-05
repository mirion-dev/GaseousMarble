if (argument_count == 1) {
    return external_call(global.gm2_internal_make_rgb, argument0 & 255, argument0 >> 8 & 255, argument0 >> 16 & 255, 1)
}

if (argument_count == 2) {
    return external_call(global.gm2_internal_make_rgb, argument0 & 255, argument0 >> 8 & 255, argument0 >> 16 & 255, argument[1])
}

if (argument_count == 3) {
    return external_call(global.gm2_internal_make_rgb, argument0, argument[1], argument[2], 1)
}

return external_call(global.gm2_internal_make_rgb, argument0, argument[1], argument[2], argument[3])
