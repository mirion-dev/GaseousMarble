<h1>GaseousMarble</h1>

**Work in progress**. Draw Unicode text in [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with high performance and rich features.

> \[!warning\]
> 
> Minimum system requirement: Windows 10, Version 1803.
> 
> Incompatible with 3D mode.

### Development

1. Install [Visual Studio 2026](https://visualstudio.microsoft.com/) with MSVC Build Tools for x64/x86 Preview.
2. Install [vcpkg](https://vcpkg.io/en/).
3. Install [DirectX 8.1 SDK](https://archive.org/details/dx81sdk_full) to `third_party/dx81`.
4. `vcpkg install wil:x86-windows`.
5. `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.
6. Build clang-format 24 from [LLVM](https://github.com/llvm/llvm-project/) if formatting is needed.

### Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)

<h1>Reference</h1>

<h2><code>gm.toml</code></h2>

| **Option** | **Default Value** | **Description** |
| --- | --- | --- |
| `atlas.texture_width` | `1024` | Texture width of glyph atlases |
| `atlas.texture_height` | `1024` | Texture height of glyph atlases |
| `atlas.max_texture_num` | `16` | Maximum number of textures for each glyph atlas |
| `raster.min_antialiasing_h_size` | `0` | Font horizontal antialiasing threshold |
| `raster.min_antialiasing_v_size` | `24` | Font vertical antialiasing threshold |
| `max_font_num` | `64` | Maximum number of fonts |
| `layout_cache_size` | `1024` | Maximum number of cached layouts |

<h2><code>gm2_new_font(key, name, properties, size, locale)</code></h2>

Add a font from system fonts or font files.

If `locale` is empty, the user default locale applies.

| **Constant** | **Value** |
| --- | --- |
| `gm2_font_weight_thin` | `100` |
| `gm2_font_weight_extra_light` | `200` |
| `gm2_font_weight_light` | `300` |
| `gm2_font_weight_normal` | `400` |
| `gm2_font_weight_medium` | `500` |
| `gm2_font_weight_semi_bold` | `600` |
| `gm2_font_weight_bold` | `700` |
| `gm2_font_weight_extra_bold` | `800` |
| `gm2_font_weight_black` | `900` |
| `gm2_font_weight_extra_black` | `950` |

| **Constant** |
| --- |
| `gm2_font_style_normal` |
| `gm2_font_style_italic` |
| `gm2_font_style_oblique` |

| **Constant** |
| --- |
| `gm2_font_stretch_ultra_condensed` |
| `gm2_font_stretch_extra_condensed` |
| `gm2_font_stretch_condensed` |
| `gm2_font_stretch_semi_condensed` |
| `gm2_font_stretch_normal` |
| `gm2_font_stretch_semi_expanded` |
| `gm2_font_stretch_expanded` |
| `gm2_font_stretch_extra_expanded` |
| `gm2_font_stretch_ultra_expanded` |

| **v1** |
| --- |
| `generate_font()` |
| `gm_font()` |

**New**: Dynamic creation and more options.

**Dropped**: Loading from sprites.

<h2>- <code>gm_free()</code><br />
- <code>gm_clear()</code></h2>

Free a font or all fonts.

**Removed**: Glyphs are extracted on demand, making manual memory management no longer necessary.

<h2><code>gm2_draw_text(x, y, text)</code><br />
<code>gm2_text_width(text)</code><br />
<code>gm2_text_height(text)</code></h2>

Draw a UTF-8 text or get its visual size.

| **v1** |
| --- |
| `gm_draw()` |
| `gm_width()` |
| `gm_height()` |

**New**: Professional typography based on DirectWrite.

<h2>+ <code>gm2_push_options()</code><br />
+ <code>gm2_pop_options()</code></h2>

Save or restore the current draw options.

<h2><code>gm2_set_font(key)</code></h2>

Set the current font.

| **v1** |
| --- |
| `gm_set_font()` |

<h2><code>gm2_get_font()</code><br />
+ <code>gm2_get_font_name()</code><br />
+ <code>gm2_get_font_weight()</code><br />
+ <code>gm2_get_font_style()</code><br />
+ <code>gm2_get_font_stretch()</code><br />
+ <code>gm2_get_font_size()</code><br />
+ <code>gm2_get_font_locale()</code></h2>

Get the current font or its properties.

| **v1** |
| --- |
| `gm_get_font()` |

<h2>+ <code>gm2_set_direction(direction)</code><br />
+ <code>gm2_set_text_direction(text_direction)</code><br />
+ <code>gm2_set_par_direction(par_direction)</code></h2>

Set the text direction or the paragraph direction.

| **Constant** |
| --- |
| `gm2_text_direction_ltr` |
| `gm2_text_direction_rtl` |

| **Constant** |
| --- |
| `gm2_par_direction_ttb` |
| `gm2_par_direction_btt` |

<h2>+ <code>gm2_get_text_direction()</code><br />
+ <code>gm2_get_par_direction()</code></h2>

Get the text direction or the paragraph direction.

<h2><code>gm2_set_alignment(alignment)</code><br />
<code>gm2_set_text_alignment(text_alignment)</code><br />
<code>gm2_set_par_alignment(par_alignment)</code></h2>

Set the text alignment or the paragraph alignment.

| **v1** |
| --- |
| `gm_set_align3()` |
| `gm_set_align()` |
| `gm_set_halign()` |
| `gm_set_valign()` |
| `gm_set_justified()` |

**Dropped**: `justified` is no longer independent of `left`/`center`/`right`.

<h2><code>gm2_get_text_alignment()</code><br />
<code>gm2_get_par_alignment()</code></h2>

Get the text alignment or the paragraph alignment.

| **Constant** |
| --- |
| `gm2_text_alignment_leading` |
| `gm2_text_alignment_center` |
| `gm2_text_alignment_trailing` |
| `gm2_text_alignment_justified` |

| **Constant** |
| --- |
| `gm2_par_alignment_near` |
| `gm2_par_alignment_center` |
| `gm2_par_alignment_far` |

| **v1** |
| --- |
| `gm_get_halign()` |
| `gm_get_valign()` |
| `gm_is_justified()` |

<h2><code>gm2_set_max_width(max_width)</code><br />
+ <code>gm2_set_max_height(max_height)</code></h2>

Set the maximum size of the text.

| **Constant** | **Value** |
| --- | --- |
| `gm2_infinity` | `1e10` |

| **v1** |
| --- |
| `gm_set_max_line_length()` |

<h2><code>gm2_get_max_width()</code><br />
+ <code>gm2_get_max_height()</code></h2>

Get the maximum size of the text.

| **v1** |
| --- |
| `gm_get_max_line_length()` |

<h2>+ <code>gm2_set_word_wrapping(word_wrapping)</code></h2>

Set the word wrapping of the text.

| **Constant** |
| --- |
| `gm2_word_wrapping_none` |
| `gm2_word_wrapping_char` |
| `gm2_word_wrapping_word` |
| `gm2_word_wrapping_whole_word` |

<h2>+ <code>gm2_get_word_wrapping()</code></h2>

Get the word wrapping of the text.

<h2>+ <code>gm2_set_trimming(trimming)</code></h2>

Set the text trimming.

| **Constant** |
| --- |
| `gm2_trimming_none` |
| `gm2_trimming_char` |
| `gm2_trimming_word` |

<h2>+ <code>gm2_get_trimming()</code></h2>

Get the text trimming.

<h2><code>gm2_set_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_uniform_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_line_height(line_height)</code><br />
<code>gm2_set_baseline(baseline)</code></h2>

Set the line spacing of the text.

| **Constant** |
| --- |
| `gm2_line_spacing_type_proportional` |
| `gm2_line_spacing_type_uniform` |

| **v1** |
| --- |
| `gm_set_line_height()` |
| `gm_set_paragraph_spacing()` |

**New**: Proportional line height.

<h2><code>gm2_get_line_spacing_type()</code><br />
<code>gm2_get_line_height()</code><br />
<code>gm2_get_baseline()</code></h2>

Get the line spacing of the text.

| **v1** |
| --- |
| `gm_get_line_height()` |
| `gm_get_paragraph_spacing()` |

<h2>+ <code>gm2_set_tab_spacing(tab_spacing)</code></h2>

Set the tab spacing of the text.

<h2>+ <code>gm2_get_tab_spacing()</code></h2>

Get the tab spacing of the text.

<h2><code>gm2_set_letter_spacing(letter_spacing)</code></h2>

Set the letter spacing of the text.

| **v1** |
| --- |
| `gm_set_letter_spacing()` |

<h2><code>gm2_get_letter_spacing()</code></h2>

Get the letter spacing of the text.

| **v1** |
| --- |
| `gm_get_letter_spacing()` |

<h2>- <code>gm_set_word_spacing()</code><br />
- <code>gm_get_word_spacing()</code></h2>

Set or get the word spacing of the text.

**Removed**: Not useful.
