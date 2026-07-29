# GaseousMarble

**Work in progress**. Draw Unicode text in [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with high performance and rich features.

> [!warning]
> Minimum system requirement: Windows 10, Version 1803.
>
> Incompatible with 3D mode.

## Functions

| **Function** | **v1 Function** | **Description** |
| :--: | :--: | -- |
| `gm2_new_font(key, name, properties, size, locale, min_aa_h_size, min_aa_v_size)` | `gm_font()`<br>`generate_font()` | <ul><li>**New**: Creates fonts dynamically and supports more font properties.</li><li>**Dropped**: Loads fonts from images.</li></ul> |
| | ~~`gm_free()`~~ | <ul><li>**Removed**: All glyphs are extracted on demand, so excessive memory usage generally does not occur.</li></ul> |
| | ~~`gm_clear()`~~ | <ul><li>**Removed**: Ditto.</li></ul> |
| `gm2_draw_text(x, y, text)`<br>`gm2_text_width(text)`<br>`gm2_text_height(text)` | `gm_draw()`<br>`gm_width()`<br>`gm_height()` | <ul><li>**New**: Professional typography based on DirectWrite.</li></ul> |
| `gm2_set_alignment(alignment)`<br>`gm2_set_text_alignment(alignment)`<br>`gm2_set_par_alignment(alignment)` | `gm_set_halign()`<br>`gm_set_valign()`<br>`gm_set_justified()`<br>`gm_set_align()`<br>`gm_set_align3()` | <ul><li>**New**: Paragraph alignment.</li><li>**Dropped**: `justified` is no longer independent of `left`/`center`/`right`.</li></ul> |
| `gm2_set_word_wrapping(word_wrapping)` | | <ul><li>**New**</li></ul> |
| `gm2_set_direction(direction)`<br>`gm2_set_text_direction(direction)`<br>`gm2_set_par_direction(direction)` | | <ul><li>**New**</li></ul> |
| `gm2_set_tab_spacing(tab_spacing)` | | <ul><li>**New**</li></ul> |
| `gm2_set_trimming(trimming)` | | <ul><li>**New**</li></ul> |
| `gm2_set_max_width(max_width)` | `gm_set_max_line_length()` | |
| `gm2_set_max_height(max_height)` | | <ul><li>**New**</li></ul> |
| `gm2_set_font(key)` | `gm_set_font()` | |
| `gm2_set_letter_spacing(letter_spacing)` | `gm_set_letter_spacing()` | |
| | ~~`gm_set_word_spacing()`~~ | <ul><li>**Removed**: Not useful.</li></ul> |
| `gm2_set_line_spacing(line_height, baseline)`<br>`gm2_set_fixed_line_spacing(line_height, baseline)`<br>`gm2_set_line_height(line_height)`<br>`gm2_set_baseline(baseline)` | `gm_set_line_height()`<br>`gm_set_paragraph_spacing()` | <ul><li>**New**: Supports proportional line height.</li></ul> |
| `gm2_get_text_alignment()`<br>`gm2_get_par_alignment()` | `gm_get_halign()`<br>`gm_get_valign()`<br>`gm_is_justified()` | |
| `gm2_get_word_wrapping()` | | |
| `gm2_get_text_direction()`<br>`gm2_get_par_direction()` | | |
| `gm2_get_tab_spacing()` | | |
| `gm2_get_trimming()` | | |
| `gm2_get_max_width()` | `gm_get_max_line_length()` | |
| `gm2_get_max_height()` | | |
| `gm2_get_font()`<br>`gm2_get_font_name()`<br>`gm2_get_font_size()`<br>`gm2_get_font_weight()`<br>`gm2_get_font_style()`<br>`gm2_get_font_stretch()`<br>`gm2_get_font_locale()`<br>`gm2_get_font_min_aa_h_size()`<br>`gm2_get_font_min_aa_v_size()` | `gm_get_font()` | |
| `gm2_get_letter_spacing()` | `gm_get_letter_spacing()` | |
| | ~~`gm_get_word_spacing()`~~ | |
| `gm2_get_line_spacing_type()`<br>`gm2_get_line_height()`<br>`gm2_get_baseline()` | `gm_get_line_height()`<br>`gm_get_paragraph_spacing()` | |

## Development

1. Install [Visual Studio 2026](https://visualstudio.microsoft.com/) with MSVC Build Tools for x64/x86 Preview.
1. Install [vcpkg](https://vcpkg.io/en/).
1. Install [DirectX 8.1 SDK](https://archive.org/details/dx81sdk_full) to `third_party/dx81`.
1. `vcpkg install wil:x86-windows`.
1. `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.
1. Build clang-format 24 from [LLVM](https://github.com/llvm/llvm-project/) if formatting is needed.

## Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)
