#set quote(block: true)
#show link: set text(fill: blue)

#title[GaseousMarble]

*Work in progress*. Draw Unicode text in #link("https://gm82.cherry-treehouse.com/#News")[GameMaker 8.2], with high performance and rich features.

#quote[
    [!warning]

    Minimum system requirement: Windows 10, Version 1803.

    Incompatible with 3D mode.
]

== Development

+ Install #link("https://visualstudio.microsoft.com/")[Visual Studio 2026] with MSVC Build Tools for x64/x86 Preview.
+ Install #link("https://vcpkg.io/en/")[vcpkg].
+ Install #link("https://archive.org/details/dx81sdk_full")[DirectX 8.1 SDK] to `third_party/dx81`.
+ `vcpkg install wil:x86-windows`.
+ `git clone -b v2 --recurse-submodules https://github.com/mirion-dev/GaseousMarble.git`.
+ Build clang-format 24 from #link("https://github.com/llvm/llvm-project/")[LLVM] if formatting is needed.

== Credits

- #link("https://github.com/snakedeveloper/gmapi")[GMAPI] (references their reverse engineering results only)
- #link("https://github.com/TeamHypersomnia/rectpack2D")[rectpack2D]
- #link("https://github.com/microsoft/wil/wiki")[WIL]

#show heading.where(level: 1): el => {
    if sys.inputs.at("x-target", default: "pdf") == "md" {
        html.h2(el.body)
    } else {
        el
    }
}

#let new = [\*New]

#title[Reference]

= `gm2_new_font(key, name, properties, size, locale, min_aa_h_size, min_aa_v_size)`

#quote[
    v1:
    - `gm_font()`
    - `generate_font()`
]

Add a font from system fonts or font files.

*New*: Dynamic creation and more options.

*Dropped*: Loading from sprites.

= `gm2_draw_text(x, y, text)`\ `gm2_text_width(text)`\ `gm2_text_height(text)`

#quote[
    v1:
    - `gm_draw()`
    - `gm_width()`
    - `gm_height()`
]

Draw a UTF-8 text or get its visual size.

*New*: Professional typography based on DirectWrite.

= `gm2_set_font(key)`

#quote[
    v1: `gm_set_font()`
]

Set the current font.

= `gm2_get_font()`\ `gm2_get_font_name()` #new\ `gm2_get_font_size()` #new\ `gm2_get_font_weight()` #new\ `gm2_get_font_style()` #new\ `gm2_get_font_stretch()` #new\ `gm2_get_font_locale()` #new\ `gm2_get_font_min_aa_h_size()` #new\ `gm2_get_font_min_aa_v_size()` #new

#quote[
    v1: `gm_get_font()`
]

Get the current font.

= `gm2_set_direction(direction)` #new\ `gm2_set_text_direction(text_direction)` #new\ `gm2_set_par_direction(par_direction)` #new

Set the text direction or the paragraph direction.

= `gm2_get_text_direction()` #new\ `gm2_get_par_direction()` #new

Get the text direction or the paragraph direction.

= `gm2_set_alignment(alignment)`\ `gm2_set_text_alignment(text_alignment)`\ `gm2_set_par_alignment(par_alignment)`

#quote[
    v1:
    - `gm_set_align3()`
    - `gm_set_align()`
    - `gm_set_halign()`
    - `gm_set_valign()`
    - `gm_set_justified()`
]

Set the text alignment or the paragraph alignment.

*Dropped*: `justified` is no longer independent of `left`/`center`/`right`.

= `gm2_get_text_alignment()`\ `gm2_get_par_alignment()`

#quote[
    v1:
    - `gm_get_halign()`
    - `gm_get_valign()`
    - `gm_is_justified()`
]

Get the text alignment or the paragraph alignment.

= `gm2_set_max_width(max_width)`

#quote[
    v1: `gm_set_max_line_length()`
]

Set the maximum width of the text.

= `gm2_get_max_width()`

#quote[
    v1: `gm_get_max_line_length()`
]

Get the maximum width of the text.

= `gm2_set_max_height(max_height)` #new

Set the maximum height of the text.

= `gm2_get_max_height()` #new

Get the maximum height of the text.

= `gm2_set_word_wrapping(word_wrapping)` #new

Set the word wrapping of the text.

= `gm2_get_word_wrapping()` #new

Get the word wrapping of the text.

= `gm2_set_trimming(trimming)` #new

Set the text trimming.

= `gm2_get_trimming()` #new

Get the text trimming.

= `gm2_set_line_spacing(line_height, baseline)`\ `gm2_set_fixed_line_spacing(line_height, baseline)`\ `gm2_set_line_height(line_height)`\ `gm2_set_baseline(baseline)`

#quote[
    v1:
    - `gm_set_line_height()`
    - `gm_set_paragraph_spacing()`
]

Set the line spacing of the text.

*New*: Proportional line height.

= `gm2_get_line_spacing_type()`\ `gm2_get_line_height()`\ `gm2_get_baseline()`

#quote[
    v1:
    - `gm_get_line_height()`
    - `gm_get_paragraph_spacing()`
]

Get the line spacing of the text.

= `gm2_set_tab_spacing(tab_spacing)` #new

Set the tab spacing of the text.

= `gm2_get_tab_spacing()` #new

Get the tab spacing of the text.

= `gm2_set_letter_spacing(letter_spacing)`

#quote[
    v1: `gm_set_letter_spacing()`
]

Set the letter spacing of the text.

= `gm2_get_letter_spacing()`

#quote[
    v1: `gm_get_letter_spacing()`
]

Get the letter spacing of the text.

= #strike[`gm_free()`\ `gm_clear()`]

Free a font or all fonts.

*Removed*: Glyphs are extracted on demand, making manual memory management no longer necessary.

= #strike[`gm_set_word_spacing()`\ `gm_get_word_spacing()`]

Set or get the word spacing of the text.

*Removed*: Not useful.
