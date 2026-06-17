# GaseousMarble

**Work in progress**. Draw Unicode text in [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with high performance and rich features.

## Functions

| **Function** | **v1 Function** | **Description** |
| :--: | :--: | -- |
| `gm2_new_font(key, name, size, properties, locale, min_aa_h_size, min_aa_v_size)` | `gm_font()`<br>`generate_font()` | <ul><li>**New**: Creates fonts dynamically and supports more font properties.</li><li>**Dropped**: Loads fonts from images.</li></ul> |
| `gm2_delete_font(key)` | `gm_free()` | |
| | ~~`gm_clear()`~~ | <ul><li>**Removed**: Originally used to clean up fonts before a game restart but later became unnecessary.</li></ul> |
| `gm2_draw_text(x, y, text)`<br>`gm2_text_width(text)`<br>`gm2_text_height(text)` | `gm_draw()`<br>`gm_width()`<br>`gm_height()` | <ul><li>**New**: Professional typography based on DirectWrite.</li></ul> |
| `gm2_set_alignment(alignment)`<br>`gm2_set_alignment_h(alignment)`<br>`gm2_set_alignment_v(alignment)` | `gm_set_halign()`<br>`gm_set_valign()`<br>`gm_set_justified()`<br>`gm_set_align()`<br>`gm_set_align3()` | <ul><li>**New**: Vertical alignment.</li><li>**Dropped**: `justified` is no longer independent of `left`/`center`/`right`.</li></ul> |
| `gm2_set_direction(direction)`<br>`gm2_set_direction_h(direction)`<br>`gm2_set_direction_v(direction)` | | <ul><li>**New**</li></ul> |
| `gm2_set_max_width(max_width)` | `gm_set_max_line_length()` | |
| `gm2_set_max_height(max_height)` | | <ul><li>**New**</li></ul> |
| `gm2_set_font(key)` | `gm_set_font()` | |
| `gm2_set_pair_kerning(use_pair_kerning)` | | <ul><li>**New**</li></ul> |
| `gm2_set_letter_spacing(letter_spacing)` | `gm_set_letter_spacing()` | |
| | ~~`gm_set_word_spacing()`~~ | <ul><li>**Removed**: Not useful.</li></ul> |
| `gm2_set_line_spacing(line_height, baseline)`<br>`gm2_set_fixed_line_spacing(line_height, baseline)`<br>`gm2_set_line_height(line_height)`<br>`gm2_set_baseline(baseline)` | `gm_set_line_height()`<br>`gm_set_paragraph_spacing()` | <ul><li>**New**: Supports proportional line height.</li></ul> |
| `gm2_get_alignment_h()`<br>`gm2_get_alignment_v()` | `gm_get_halign()`<br>`gm_get_valign()`<br>`gm_is_justified()` | |
| `gm2_get_direction_h()`<br>`gm2_get_direction_v()` | | <ul><li>**New**</li></ul> |
| `gm2_get_max_width()` | `gm_get_max_line_length()` | |
| `gm2_get_max_height()` | | <ul><li>**New**</li></ul> |
| `gm2_get_font()` | `gm_get_font()` | |
| `gm2_get_font_name()`<br>`gm2_get_font_size()`<br>`gm2_get_font_weight()`<br>`gm2_get_font_style()`<br>`gm2_get_font_stretch()`<br>`gm2_get_font_locale()`<br>`gm2_get_font_min_aa_h_size()`<br>`gm2_get_font_min_aa_v_size()` | | <ul><li>**New**</li></ul> |
| `gm2_use_pair_kerning()` | | <ul><li>**New**</li></ul> |
| `gm2_get_letter_spacing()` | `gm_get_letter_spacing()` | |
| | ~~`gm_get_word_spacing()`~~ | <ul><li>**Removed**: Not useful.</li></ul> |
| `gm2_is_fixed_line_spacing()`<br>`gm2_get_line_height()`<br>`gm2_get_baseline()` | `gm_get_line_height()`<br>`gm_get_paragraph_spacing()` | |

## Development

1. Install [Visual Studio 2026](https://visualstudio.microsoft.com/) and [vcpkg](https://vcpkg.io/en/).
1. Install [DirectX 8.1 SDK](https://archive.org/details/dx81sdk_full) to `third_party/dx81`.
1. `vcpkg install wil:x86-windows`.
1. `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.

## Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)
