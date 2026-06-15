# GaseousMarble

**Work in progress**. Draw Unicode text in [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with high performance and rich features.

## Functions

`gm2_new_font(key, name, size, properties, locale, min_antialiasing_h_size, min_antialiasing_v_size)`

- v1: <br>
  `gm_font()`<br>
  `generate_font(): font_path, sprite_path, font_size, charset, dense, smoothing`
- New: Creates fonts dynamically and supports more font properties.
- Dropped: Loads fonts from images.

`gm2_delete_font(key)`

- v1: `gm_free()`

~~`gm_clear()`~~

- Removed: Originally used to clean up fonts before a game restart but later became unnecessary.

`gm2_draw_text(x, y, text)`<br>
`gm2_text_width(text)`<br>
`gm2_text_height(text)`

- v1: <br>
  `gm_draw()`<br>
  `gm_width()`<br>
  `gm_height()`
- New: Prefessional typography based on DirectWrite.

`gm2_set_alignment(alignment)`<br>
`gm2_set_alignment_h(alignment)`<br>
`gm2_set_alignment_v(alignment)`

- v1: <br>
  `gm_set_halign()`<br>
  `gm_set_valign()`<br>
  `gm_set_justified()`<br>
  `gm_set_align()`<br>
  `gm_set_align3()`
- New: Vertical alignment.
- Dropped: `justified` is no longer independent of `left`/`center`/`right`.

`gm2_set_max_width(max_width)`

- v1: `gm_set_max_line_length()`

`gm2_set_max_height(max_height)`

- New

`gm2_set_font(key)`

- v1: `gm_set_font()`

`gm2_set_letter_spacing(letter_spacing)`

- v1: `gm_set_letter_spacing()`

~~`gm_set_word_spacing()`~~

- Removed: Not useful.

`gm2_set_line_spacing(line_height, baseline)`<br>
`gm2_set_fixed_line_spacing(line_height, baseline)`<br>
`gm2_set_line_height(line_height)`<br>
`gm2_set_baseline(baseline)`

- v1: <br>
  `gm_set_line_height()`<br>
  `gm_set_paragraph_spacing()`
- New: Supports proportional line height.

`gm2_get_alignment()`<br>
`gm2_get_alignment_h()`<br>
`gm2_get_alignment_v()`

- v1: <br>
  `gm_get_halign()`<br>
  `gm_get_valign()`<br>
  `gm_is_justified()`

`gm2_get_max_width()`

- v1: `gm_get_max_line_length()`

`gm2_get_max_height()`

- New

`gm2_get_font()`

- v1: `gm_get_font()`

`gm2_get_letter_spacing()`

- v1: `gm_get_letter_spacing()`

~~`gm_get_word_spacing()`~~

- Removed: Not useful.

`gm2_is_fixed_line_spacing()`<br>
`gm2_get_line_height()`<br>
`gm2_get_baseline()`

- v1: <br>
  `gm_get_line_height()`<br>
  `gm_get_paragraph_spacing()`

## Development

1. Install [Visual Studio 2026](https://visualstudio.microsoft.com/) and [vcpkg](https://vcpkg.io/en/).
1. Install [DirectX 8.1 SDK](https://archive.org/details/dx81sdk_full) to `third_party/dx81`.
1. `vcpkg install wil:x86-windows`.
1. `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.

## Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)
