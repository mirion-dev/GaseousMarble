# GaseousMarble

Draw Unicode texts in GameMaker 8.2, 1.75x faster than [FoxWriting](https://github.com/Noisyfox/FoxWriting).

## Usage

GaseousMarble provides following functions for drawing texts

| **Function** | **Description** |
| -- | -- |
| `gm_font(font_name, sprite_path)` | Adds a font. The font sprite and glyph data can be generated using `tools/generate_font.py`. |
| `gm_free(font_name)` | Frees a font. |
| `gm_clear()` | Frees all fonts. |
| `gm_draw(x, y, text)` | Draws a UTF-8 string. |
| `gm_width(text)` | Returns the width of the text. |
| `gm_height(text)` | Returns the height of the text. |

as well as setters and getters for configuring the drawing parameters

| **Setter** | **Getter** |
| -- | -- |
| `gm_set_font(font_name)` | `gm_get_font()` |
| `gm_set_color(color)`<br>`gm_set_color2(color_top, color_bottom)` | `gm_get_color_top()`<br>`gm_get_color_bottom()` |
| `gm_set_alpha(alpha)` | `gm_get_alpha()` |
| `gm_set_halign(align)`<br>`gm_set_valign(align)`<br>`gm_set_justified(justified)`<br>`gm_set_align(halign, valign)`<br>`gm_set_align3(halign, valign, justified)` | `gm_get_halign()`<br>`gm_get_valign()`<br>`gm_is_justified()` |
| `gm_set_letter_spacing(spacing)` | `gm_get_letter_spacing()` |
| `gm_set_word_spacing(spacing)` | `gm_get_word_spacing()` |
| `gm_set_paragraph_spacing(spacing)` | `gm_get_paragraph_spacing()` |
| `gm_set_line_height(height)` | `gm_get_line_height()` |
| `gm_set_max_line_length(length)` | `gm_get_max_line_length()` |
| `gm_set_offset(x, y)` | `gm_get_offset_x()`<br>`gm_get_offset_y()` |
| `gm_set_scale(x, y)` | `gm_get_scale_x()`<br>`gm_get_scale_y()` |

If any operation fails (for example, due to invalid arguments), the corresponding function will return `false`.

## Credits

This project was inspired by the following open-source projects. Their code wasn't directly copied but was adapted and modified to better suit the needs of this project. Thank you to the developers for their efforts and contributions.

| **Project** | **Referenced** |
| -- | -- |
| [FoxWriting](https://github.com/Noisyfox/FoxWriting) | Implementation of text drawing |
| [GMAPI](https://github.com/snakedeveloper/gmapi) | Engine internal data structures |
| [GMAPI 8.1](https://github.com/gm-archive/gmapi-8.1) | Magic numbers for engine resources |
