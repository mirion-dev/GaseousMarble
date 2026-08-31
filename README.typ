#set quote(block: true)
#show link: set text(fill: blue)

#title[GaseousMarble]

*Work in progress*. Draw Unicode text in #link("https://gm82.cherry-treehouse.com/#News")[GameMaker 8.2], with high performance and rich features.

#quote[
    [!warning]

    Minimum system requirement: Windows 10, Version 1803.
]

== Development

+ Install #link("https://visualstudio.microsoft.com/")[Visual Studio 2026] with MSVC Build Tools for x64/x86 Preview.
+ Install #link("https://vcpkg.io/en/")[vcpkg].
+ Install #link("https://archive.org/details/dx81sdk_full")[DirectX 8.1 SDK] to `third_party/dx81`.
+ `vcpkg install --triplet x86-windows-static-md glaze spdlog wil`.
+ `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.
+ Build clang-format 24 from #link("https://github.com/llvm/llvm-project/")[LLVM] if formatting is needed.

== Credits

- #link("https://github.com/snakedeveloper/gmapi")[GMAPI] (references their reverse engineering results only)
- #link("https://github.com/stephenberry/glaze")[Glaze]
- #link("https://github.com/gabime/spdlog")[spdlog]
- #link("https://github.com/TeamHypersomnia/rectpack2D")[rectpack2D]
- #link("https://github.com/microsoft/wil/wiki")[WIL]

#show heading.where(level: 1): el => {
    if sys.inputs.at("x-target", default: "pdf") == "md" {
        html.h2(el.body)
    } else {
        el
    }
}

#show list: el => {
    table(
        table.header[*v1*],
        ..el.children.map(i => i.body),
    )
}

#title[Reference]

= `gm2.toml`

#table(
    columns: 3,
    table.header[*Option*][*Default Value*][*Description*],
    `atlas.texture_width`, `1024`, [Texture width of glyph atlases],
    `atlas.texture_height`, `1024`, [Texture height of glyph atlases],
    `atlas.max_texture_num`, `16`, [Maximum number of textures for each glyph atlas],
    `raster.min_antialiasing_h_size`, `0`, [Font horizontal antialiasing threshold],
    `raster.min_antialiasing_v_size`, `24`, [Font vertical antialiasing threshold],
    `max_font_num`, `64`, [Maximum number of fonts],
    `layout_cache_size`, `1024`, [Maximum number of cached layouts],
)

= `gm2_new_font(key, name, properties, size, locale)`

Add a font from system fonts or font files.

If `locale` is empty, the user's default locale applies.

#table(
    columns: 2,
    table.header[*Constant*][*Value*],
    `gm2_font_weight_thin`, `100`,
    `gm2_font_weight_extra_light`, `200`,
    `gm2_font_weight_light`, `300`,
    `gm2_font_weight_normal`, `400`,
    `gm2_font_weight_medium`, `500`,
    `gm2_font_weight_semi_bold`, `600`,
    `gm2_font_weight_bold`, `700`,
    `gm2_font_weight_extra_bold`, `800`,
    `gm2_font_weight_black`, `900`,
    `gm2_font_weight_extra_black`, `950`,
)

#table(
    table.header[*Constant*],
    `gm2_font_style_normal`,
    `gm2_font_style_italic`,
    `gm2_font_style_oblique`,
)

#table(
    table.header[*Constant*],
    `gm2_font_stretch_ultra_condensed`,
    `gm2_font_stretch_extra_condensed`,
    `gm2_font_stretch_condensed`,
    `gm2_font_stretch_semi_condensed`,
    `gm2_font_stretch_normal`,
    `gm2_font_stretch_semi_expanded`,
    `gm2_font_stretch_expanded`,
    `gm2_font_stretch_extra_expanded`,
    `gm2_font_stretch_ultra_expanded`,
)

- `generate_font()`
- `gm_font()`

*New*: Dynamic creation and more options.

*Dropped*: Loading from sprites.

= - `gm_free()`\ - `gm_clear()`

Free a font or all fonts.

*Removed*: Glyphs are extracted on demand, making manual memory management no longer necessary.

= `gm2_draw_text(x, y, text)`\ `gm2_text_width(text)`\ `gm2_text_height(text)`

Draw a UTF-8 text or get its visual size.

- `gm_draw()`
- `gm_width()`
- `gm_height()`

*New*: Professional typography based on DirectWrite.

= + `gm2_push_options()`\ + `gm2_pop_options()`

Save or restore the current draw options.

= `gm2_set_font(key)`\ `gm2_get_font()`\ + `gm2_get_font_name()`\ + `gm2_get_font_weight()`\ + `gm2_get_font_style()`\ + `gm2_get_font_stretch()`\ + `gm2_get_font_size()`\ + `gm2_get_font_locale()`

Set or get the current font.

- `gm_set_font()`
- `gm_get_font()`

= + `gm2_set_direction(direction)`\ + `gm2_set_text_direction(text_direction)`\ + `gm2_set_par_direction(par_direction)`\ + `gm2_get_text_direction()`\ + `gm2_get_par_direction()`

Set or get the text direction and the paragraph direction.

#table(
    table.header[*Constant*],
    `gm2_text_direction_ltr`,
    `gm2_text_direction_rtl`,
)

#table(
    table.header[*Constant*],
    `gm2_par_direction_ttb`,
    `gm2_par_direction_btt`,
)

= `gm2_set_alignment(alignment)`\ `gm2_set_text_alignment(text_alignment)`\ `gm2_set_par_alignment(par_alignment)`\ `gm2_get_text_alignment()`\ `gm2_get_par_alignment()`

Set or get the text alignment and the paragraph alignment.

#table(
    table.header[*Constant*],
    `gm2_text_alignment_leading`,
    `gm2_text_alignment_center`,
    `gm2_text_alignment_trailing`,
    `gm2_text_alignment_justified`,
)

#table(
    table.header[*Constant*],
    `gm2_par_alignment_near`,
    `gm2_par_alignment_center`,
    `gm2_par_alignment_far`,
)

- `gm_set_align3()`
- `gm_set_align()`
- `gm_set_halign()`
- `gm_set_valign()`
- `gm_set_justified()`
- `gm_get_halign()`
- `gm_get_valign()`
- `gm_is_justified()`

*Dropped*: `justified` is no longer independent of `left`/`center`/`right`.

= + `gm2_set_max_size(max_width, max_height)`\ `gm2_set_max_width(max_width)`\ + `gm2_set_max_height(max_height)`\ `gm2_get_max_width()`\ + `gm2_get_max_height()`

Set or get the maximum size of the text.

If the maximum size is 0, the alignment point is the drawing point; otherwise, it is the text box.

#table(
    columns: 2,
    table.header[*Constant*][*Value*],
    `gm2_infinity`, `1e5`,
)

- `gm_set_max_line_length()`
- `gm_get_max_line_length()`

= + `gm2_set_word_wrapping(word_wrapping)`\ + `gm2_get_word_wrapping()`

Set or get the word wrapping of the text.

#table(
    table.header[*Constant*],
    `gm2_word_wrapping_none`,
    `gm2_word_wrapping_char`,
    `gm2_word_wrapping_word`,
    `gm2_word_wrapping_whole_word`,
)

= + `gm2_set_trimming(trimming)`\ + `gm2_get_trimming()`

Set or get the text trimming.

#table(
    table.header[*Constant*],
    `gm2_trimming_none`,
    `gm2_trimming_char`,
    `gm2_trimming_word`,
)

= + `gm2_set_line_spacing(line_height, baseline)`\ `gm2_set_uniform_line_spacing(line_height, baseline)`\ `gm2_set_line_height(line_height)`\ `gm2_set_baseline(baseline)`\ + `gm2_get_line_spacing_type()`\ `gm2_get_line_height()`\ `gm2_get_baseline()`

Set or get the line spacing of the text.

#table(
    table.header[*Constant*],
    `gm2_line_spacing_type_proportional`,
    `gm2_line_spacing_type_uniform`,
)

- `gm_set_line_height()`
- `gm_set_paragraph_spacing()`
- `gm_get_line_height()`
- `gm_get_paragraph_spacing()`

*New*: Proportional line height.

= + `gm2_set_tab_spacing(tab_spacing)`\ + `gm2_get_tab_spacing()`

Set or get the tab spacing of the text.

= `gm2_set_letter_spacing(letter_spacing)`\ `gm2_get_letter_spacing()`

Set or get the letter spacing of the text.

- `gm_set_letter_spacing()`
- `gm_get_letter_spacing()`

= - `gm_set_word_spacing()`\ - `gm_get_word_spacing()`

Set or get the word spacing of the text.

*Removed*: Not useful.
