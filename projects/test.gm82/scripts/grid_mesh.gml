var xx{ xx = argument[0] }
var yy{ yy = argument[1] }
var row_num{ row_num = argument[2] }
var col_num{ col_num = argument[3] }
var width{ width = argument[4] }
var height{ height = argument[5] }
var row_header_size{ row_header_size = argument[6] }
var col_header_size{ col_header_size = argument[7] }
var color{ color = argument[8] }

grid_mesh_cell_width = (width - row_header_size) / col_num
grid_mesh_cell_height = (height - col_header_size) / row_num

grid_mesh_row[0] = yy + col_header_size / 2
draw_line_color(xx, yy, xx + width, yy, color, color)
draw_line_color(xx, yy + col_header_size, xx + width, yy + col_header_size, color, color)

var row{}
for (row = 1; row <= row_num; row += 1) {
    grid_mesh_row[row] = yy + col_header_size + (row - .5) * grid_mesh_cell_height
    draw_line_color(xx, yy + col_header_size + row * grid_mesh_cell_height, xx + width, yy + col_header_size + row * grid_mesh_cell_height, color, color)
}

grid_mesh_col[0] = xx + row_header_size / 2
draw_line_color(xx, yy, xx, yy + height, color, color)
draw_line_color(xx + row_header_size, yy, xx + row_header_size, yy + height, color, color)

var col{}
for (col = 1; col <= col_num; col += 1) {
    grid_mesh_col[col] = xx + row_header_size + (col - .5) * grid_mesh_cell_width
    draw_line_color(xx + row_header_size + col * grid_mesh_cell_width, yy, xx + row_header_size + col * grid_mesh_cell_width, yy + height, color, color)
}
