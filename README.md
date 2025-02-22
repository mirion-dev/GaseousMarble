# GaseousMarble

Draw Unicode texts in GameMaker 8.2.

## Usage

GaseousMarble provides following functions for drawing texts

| **Function** | **Description** |
| -- | -- |
| `gm_font(name, sprite_path, glyph_path)` | Adds a font. The font sprite and glyph data can be generated using `tools/font_generator.py`. |
| `gm_width(text)` | Returns the width of the text. |
| `gm_height(text)` | Returns the height of the text. |
| `gm_free(name)` | Frees a specified font. |
| `gm_clear()` | Frees all loaded fonts. |
| `gm_draw(x, y, text)` | Draws a UTF-8 string. |

as well as setters and getters for configuring the drawing parameters

| **Setter** | **Getter** |
| -- | -- |
| `gm_set_font(name)` | `gm_get_font()` |
| `gm_set_color(color)`<br>`gm_set_color2(color_top, color_bottom)` | `gm_get_color_top()`<br>`gm_get_color_bottom()` |
| `gm_set_alpha(alpha)` | `gm_get_alpha()` |
| `gm_set_halign(align)`<br>`gm_set_valign(align)`<br>`gm_set_align(halign, valign)` | `gm_get_halign()`<br>`gm_get_valign()` |
| `gm_set_max_line_width(max_width)` | `gm_get_max_line_width()` |
| `gm_set_letter_spacing(spacing)` | `gm_get_letter_spacing()` |
| `gm_set_word_spacing(spacing)` | `gm_get_word_spacing()` |
| `gm_set_line_height(height)` | `gm_get_line_height()` |
| `gm_set_offset(x, y)`| `gm_get_offset_x()`<br>`gm_get_offset_y()` |
| `gm_set_scale(x, y)` | `gm_get_scale_x()`<br>`gm_get_scale_y()` |

If any operation fails (for example, due to invalid arguments), the corresponding function will return `false`.

## Development

1. Install DirectX 8.1 SDK to the `dx81` folder.
1. Open the project in Visual Studio 2022.

The current drawing mechanism of GaseousMarble relies on a Python script to generate sprite images and glyph data, which are then rendered using GameMaker's built-in functions to draw portions of the sprites. However, this approach lacks flexibility. To address this, I plan to extract textures directly from the font file, map them onto a dedicated sprite, and render the sprite instead.

## Credits

This project was inspired by the following open-source projects. Their code wasn't directly copied but was adapted and modified to better suit the needs of this project. Thank you to the developers for their efforts and contributions.

| **Project** | **Referenced** |
| -- | -- |
| [FoxWriting](https://github.com/Noisyfox/FoxWriting) | Implemention of text drawing |
| [GMAPI](https://github.com/snakedeveloper/gmapi) | Engine internal data structures |
| [GMAPI 8.1](https://github.com/gm-archive/gmapi-8.1) | Magic numbers for engine resources |
