var xx{ xx = argument0 }
var yy{ yy = argument1 }
var row_num{ row_num = argument2 }
var col_num{ col_num = argument3 }
var width{ width = argument4 }
var height{ height = argument5 }
var row_header_size{ row_header_size = argument6 }
var col_header_size{ col_header_size = argument7 }
var color{ color = argument8 }

global.grid_mesh_cell_width = (width - row_header_size) / col_num
global.grid_mesh_cell_height = (height - col_header_size) / row_num

global.grid_mesh_row[0] = yy + col_header_size / 2
draw_line_color(xx, yy, xx + width, yy, color, color)
draw_line_color(xx, yy + col_header_size, xx + width, yy + col_header_size, color, color)

var row{}
for (row = 1; row <= row_num; row += 1) {
    global.grid_mesh_row[row] = yy + col_header_size + (row - .5) * global.grid_mesh_cell_height
    draw_line_color(xx, yy + col_header_size + row * global.grid_mesh_cell_height, xx + width, yy + col_header_size + row * global.grid_mesh_cell_height, color, color)
}

global.grid_mesh_col[0] = xx + row_header_size / 2
draw_line_color(xx, yy, xx, yy + height, color, color)
draw_line_color(xx + row_header_size, yy, xx + row_header_size, yy + height, color, color)

var col{}
for (col = 1; col <= col_num; col += 1) {
    global.grid_mesh_col[col] = xx + row_header_size + (col - .5) * global.grid_mesh_cell_width
    draw_line_color(xx + row_header_size + col * global.grid_mesh_cell_width, yy, xx + row_header_size + col * global.grid_mesh_cell_width, yy + height, color, color)
}
