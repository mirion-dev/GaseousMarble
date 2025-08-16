> [!Warning]
> You are viewing the readme for the `master` branch, which may contain unreleased features. For a specific version, please select the corresponding tag.
 
# GaseousMarble

Draw Unicode texts in GameMaker 8.2, 80% faster than [FoxWriting](https://github.com/Noisyfox/FoxWriting) and support more options.

## Usage

GaseousMarble provides following functions for drawing texts

| **Function** | **Description** | **Returned Value** |
| -- | -- | -- |
| `gm_font(font_name, sprite_path)` | Add a font. The font sprite and data file can be generated using `tools/generate_font.py`. | `1` - Font already exists<br>`0` - OK<br>`-1` - Data file not found<br>`-2` - Invalid data file header<br>`-3` - Data file is corrupt<br>`-4` - Failed to add sprite |
| `gm_free(font_name)` | Free a font. | `1` - Font not found<br>`0` - OK |
| `gm_clear()` | Free all fonts. | Always OK |
| `gm_draw(x, y, text)` | Draw a UTF-8 string. | `0` - OK<br>`-1` - Font not set<br>`-2` - Failed to measure |
| `gm_width(text)` | Return the width of the text. | `>= 0` - OK<br>`< 0` - Failed to measure |
| `gm_height(text)` | Return the height of the text. | `>= 0` - OK<br>`< 0` - Failed to measure |

as well as setters and getters for configuring the drawing setting

| **Setter** | **Getter** | **Setter Returned Value** |
| -- | -- | -- |
| `gm_set_font(font_name)` | `gm_get_font()` | `0` - OK<br>`-1` - Font not found |
| `gm_set_color(color)`<br>`gm_set_color2(color_top, color_bottom)` | `gm_get_color_top()`<br>`gm_get_color_bottom()` | Always OK |
| `gm_set_alpha(alpha)` | `gm_get_alpha()` | `0` - OK<br>`-1` - Invalid argument |
| `gm_set_halign(align)`<br>`gm_set_valign(align)`<br>`gm_set_justified(justified)`<br>`gm_set_align(halign, valign)`<br>`gm_set_align3(halign, valign, justified)` | `gm_get_halign()`<br>`gm_get_valign()`<br>`gm_is_justified()` | Always OK |
| `gm_set_letter_spacing(spacing)` | `gm_get_letter_spacing()` | Always OK |
| `gm_set_word_spacing(spacing)` | `gm_get_word_spacing()` | Always OK |
| `gm_set_paragraph_spacing(spacing)` | `gm_get_paragraph_spacing()` | Always OK |
| `gm_set_line_height(height)` | `gm_get_line_height()` | `0` - OK<br>`-1` - Invalid argument |
| `gm_set_max_line_length(length)` | `gm_get_max_line_length()` | `0` - OK<br>`-1` - Invalid argument |
| `gm_set_offset(x, y)` | `gm_get_offset_x()`<br>`gm_get_offset_y()` | Always OK |
| `gm_set_scale(x, y)` | `gm_get_scale_x()`<br>`gm_get_scale_y()` | `0` - OK<br>`-1` - Invalid argument(s) |

## Credits

This project utilizes data from the reverse-engineering efforts of the following projects. Thanks to the original authors for their work.

| **Project** | **Reference** |
| -- | -- |
| [GMAPI](https://github.com/snakedeveloper/gmapi) | Data structures |
| [GMAPI 8.1](https://github.com/gm-archive/gmapi-8.1) | Magic numbers |
