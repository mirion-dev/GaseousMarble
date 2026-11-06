<h1>GaseousMarble</h1>

Professional, high-performance text rendering for [GameMaker 8.2](https://gm82.cherry-treehouse.com/#News), with 80+ functions covering layout control, arbitrary transformations, and multi-directional gradients.

The example project is located at `projects/example.gm82`. You may need to place a [suitable font](https://github.com/adobe-fonts/source-han-serif/releases) named `default.ttf` alongside the built game to display CJK characters correctly.

> \[!warning\]
> 
> Minimum system requirement: Windows 10, Version 1709.

### Roadmap

- Support text effects.
- Support vertical writing.

### Development

1. `git clone --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.
2. Install [DirectX 8.1b SDK](https://archive.org/details/dx81b_sdk) to `third_party/dx8.1b`.
3. Install [DirectX 9.0c SDK](https://archive.org/details/dxsdk_jun10) to `third_party/dx9.0c`.

### Credits

- [GMAPI](https://github.com/snakedeveloper/gmapi) (references their reverse engineering results only)
- [Eigen](https://gitlab.com/libeigen/eigen)
- [Glaze](https://github.com/stephenberry/glaze)
- [spdlog](https://github.com/gabime/spdlog)
- [rectpack2D](https://github.com/TeamHypersomnia/rectpack2D)
- [WIL](https://github.com/microsoft/wil/wiki)

<h1>Reference</h1>

<h2><code>gm2.toml</code></h2>

| **Option** | **Default Value** | **Description** |
| --- | --- | --- |
| `atlas.texture_width` | `1024` | Texture width of glyph atlases |
| `atlas.texture_height` | `1024` | Texture height of glyph atlases |
| `atlas.max_texture_num` | `16` | Maximum number of textures for each glyph atlas |
| `raster.min_antialiasing_size_h` | `0` | Font horizontal antialiasing threshold |
| `raster.min_antialiasing_size_v` | `0` | Font vertical antialiasing threshold |
| `layout_cache_size` | `1024` | Maximum number of cached layouts |

<h2><code>gm2_new_font(key, name, size, properties = normal, locale = "")</code></h2>

Add a font from system fonts or font files. `key` should be **non-empty**.

Adding a font to an existing key still **requires** setting the font.

If `locale` is empty, the user’s default locale applies.

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

<h2><code>gm2_delete_font(key)</code></h2>

Remove a font.

<h2><code>gm2_draw_text(x, y, text)</code><br />
<code>gm2_get_aabb_x(text)</code><br />
<code>gm2_get_aabb_y(text)</code><br />
<code>gm2_get_aabb_width(text)</code><br />
<code>gm2_get_aabb_height(text)</code></h2>

Draw a UTF-8 text or get its **layout** AABB.

<h2><code>gm2_save_options(key)</code><br />
<code>gm2_load_options(key)</code></h2>

Save or load the current draw options.

<h2><code>gm2_make_rgb(rgb, alpha = 1)</code><br />
<code>gm2_make_rgb(red, green, blue, alpha = 1)</code><br />
<code>gm2_make_hsv(hue, saturation, value, alpha = 1)</code></h2>

Create a color. The range of `red`, `green` and `blue` is **0 to 255**. The range of `hue` is **0 to 360**. The range of `saturation`, `value` and `alpha` is **0 to 1**.

<h2><code>gm2_set_font(key)</code><br />
<code>gm2_get_font()</code><br />
<code>gm2_get_font_name()</code><br />
<code>gm2_get_font_weight()</code><br />
<code>gm2_get_font_style()</code><br />
<code>gm2_get_font_stretch()</code><br />
<code>gm2_get_font_size()</code><br />
<code>gm2_get_font_locale()</code></h2>

Set or get the current font.

<h2><code>gm2_set_direction(direction)</code><br />
<code>gm2_set_text_direction(text_direction)</code><br />
<code>gm2_set_par_direction(par_direction)</code><br />
<code>gm2_get_text_direction()</code><br />
<code>gm2_get_par_direction()</code></h2>

Set or get the text direction and the paragraph direction.

| **Constant** |
| --- |
| `gm2_text_direction_ltr` |
| `gm2_text_direction_rtl` |

| **Constant** |
| --- |
| `gm2_par_direction_ttb` |
| `gm2_par_direction_btt` |

<h2><code>gm2_set_alignment(alignment)</code><br />
<code>gm2_set_text_alignment(text_alignment)</code><br />
<code>gm2_set_par_alignment(par_alignment)</code><br />
<code>gm2_get_text_alignment()</code><br />
<code>gm2_get_par_alignment()</code></h2>

Set or get the text alignment and the paragraph alignment.

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

<h2><code>gm2_set_max_size(max_width, max_height)</code><br />
<code>gm2_set_max_width(max_width)</code><br />
<code>gm2_set_max_height(max_height)</code><br />
<code>gm2_get_max_width()</code><br />
<code>gm2_get_max_height()</code></h2>

Set or get the maximum size of the text.

When the maximum size is 0, the text model is point-based and aligns to the drawing point; otherwise, it is box-based and aligns to the box.

| **Constant** | **Value** |
| --- | --- |
| `gm2_infinity` | `1e5` |

<h2><code>gm2_set_word_wrapping(word_wrapping)</code><br />
<code>gm2_get_word_wrapping()</code></h2>

Set or get the word wrapping of the text.

| **Constant** |
| --- |
| `gm2_word_wrapping_none` |
| `gm2_word_wrapping_char` |
| `gm2_word_wrapping_word` |
| `gm2_word_wrapping_whole_word` |

<h2><code>gm2_set_trimming(trimming)</code><br />
<code>gm2_get_trimming()</code></h2>

Set or get the text trimming.

| **Constant** |
| --- |
| `gm2_trimming_none` |
| `gm2_trimming_char` |
| `gm2_trimming_word` |

<h2><code>gm2_set_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_uniform_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_line_height(line_height)</code><br />
<code>gm2_set_baseline(baseline)</code><br />
<code>gm2_get_line_spacing_type()</code><br />
<code>gm2_get_line_height()</code><br />
<code>gm2_get_baseline()</code></h2>

Set or get the line spacing of the text.

| **Constant** |
| --- |
| `gm2_line_spacing_type_proportional` |
| `gm2_line_spacing_type_uniform` |

<h2><code>gm2_set_tab_spacing(tab_spacing)</code><br />
<code>gm2_get_tab_spacing()</code></h2>

Set or get the tab spacing of the text.

<h2><code>gm2_set_letter_spacing(letter_spacing)</code><br />
<code>gm2_get_letter_spacing()</code></h2>

Set or get the letter spacing of the text.

<h2><code>gm2_set_origin(x, y)</code><br />
<code>gm2_get_origin_x()</code><br />
<code>gm2_get_origin_y()</code></h2>

Set or get the **normalized** origin of the text. Only applies to the **box** model.

<h2><code>gm2_set_transform()</code><br />
<code>gm2_set_transform(a00, a01, a10, a11)</code><br />
<code>gm2_set_transform(a00, a01, a10, a11, dx, dy)</code><br />
<code>gm2_add_transform(a00, a01, a10, a11)</code><br />
<code>gm2_add_transform(a00, a01, a10, a11, dx, dy)</code><br />
<code>gm2_add_translation(x, y)</code><br />
<code>gm2_add_translation_x(x)</code><br />
<code>gm2_add_translation_y(y)</code><br />
<code>gm2_add_scale(x, y)</code><br />
<code>gm2_add_scale_x(x)</code><br />
<code>gm2_add_scale_y(y)</code><br />
<code>gm2_add_rotation(theta)</code><br />
<code>gm2_add_skew(x, y)</code><br />
<code>gm2_add_skew_x(x)</code><br />
<code>gm2_add_skew_y(y)</code><br />
<code>gm2_get_transform_a00()</code><br />
<code>gm2_get_transform_a01()</code><br />
<code>gm2_get_transform_a10()</code><br />
<code>gm2_get_transform_a11()</code><br />
<code>gm2_get_transform_dx()</code><br />
<code>gm2_get_transform_dy()</code></h2>

Set or get the text transformation. Angles are in **radians**.

<h2><code>gm2_set_color(color)</code><br />
<code>gm2_set_color(top, bottom)</code><br />
<code>gm2_set_color(tl, tr, bl, br)</code><br />
<code>gm2_get_color_tl()</code><br />
<code>gm2_get_color_tr()</code><br />
<code>gm2_get_color_bl()</code><br />
<code>gm2_get_color_br()</code></h2>

Set or get the text color. The alpha channel is **respected**. See `gm2_make_rgb()` and `gm2_make_hsv()`.

<h2><code>gm2_set_opacity(opacity)</code><br />
<code>gm2_get_opacity()</code></h2>

Set or get the text opacity.
