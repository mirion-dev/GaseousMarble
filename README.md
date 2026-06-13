# GaseousMarble

**Work in progress**. Draw Unicode text in [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with high performance and rich features.

## Functions

`gm_new_font(key, name, size, weight, style, stretch, locale)`

- v1: `gm_font()`
- New: Dynamically creates fonts and supports more font properties.
- To be implemented: Support loading from font files.

`gm_delete_font(key)`

- v1: `gm_free()`

Removed

- v1: `gm_clear()`
- Reason: Originally used to clean up fonts before a game restart but later became unnecessary.

`gm_draw_text(x, y, text)`

- v1: `gm_draw()`
- New: Supports RTL languages.

`gm_set_alignment(alignment)`<br>
`gm_set_alignment_h(alignment)`<br>
`gm_set_alignment_v(alignment)`

- v1: <br>
  `gm_set_halign()`<br>
  `gm_set_valign()`<br>
  `gm_set_justified()`<br>
  `gm_set_align()`<br>
  `gm_set_align3()`
- Changes: Based on the text box instead of the drawing point. `justified` is no longer independent of `left`/`center`/`right`.

`gm_set_max_width(max_width)`

- v1: `gm_set_max_line_length()`

`gm_set_max_height(max_height)`

- v1: None

`gm_set_font(key)`

- v1: `gm_set_font()`

`gm_get_alignment()`<br>
`gm_get_alignment_h()`<br>
`gm_get_alignment_v()`

- v1: <br>
  `gm_get_halign()`<br>
  `gm_get_valign()`<br>
  `gm_is_justified()`

`gm_get_max_width()`

- v1: `gm_get_max_line_length()`

`gm_get_max_height()`

- v1: None

`gm_get_font()`

- v1: `gm_get_font()`

## Development

1. Install [Visual Studio 2026](https://visualstudio.microsoft.com/) and [vcpkg](https://vcpkg.io/en/).
1. Install [DirectX 8.1 SDK](https://archive.org/details/dx81sdk_full) to `third_party/dx81`.
1. `vcpkg install wil:x86-windows`.
1. `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.

## Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)
