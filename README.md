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

<h2><code>gm2_new_font(key, name, properties, size, locale, min_aa_h_size, min_aa_v_size)</code></h2>

> v1:
> 
> - `gm_font()`
> - `generate_font()`
> 

Add a font from system fonts or font files.

**New**: Dynamic creation and more options.

**Dropped**: Loading from sprites.

<h2><code>gm2_draw_text(x, y, text)</code><br />
<code>gm2_text_width(text)</code><br />
<code>gm2_text_height(text)</code></h2>

> v1:
> 
> - `gm_draw()`
> - `gm_width()`
> - `gm_height()`
> 

Draw a UTF-8 text or get its visual size.

**New**: Professional typography based on DirectWrite.

<h2><code>gm2_set_font(key)</code></h2>

> v1: `gm_set_font()`

Set the current font.

<h2><code>gm2_get_font()</code><br />
<code>gm2_get_font_name()</code> *New<br />
<code>gm2_get_font_size()</code> *New<br />
<code>gm2_get_font_weight()</code> *New<br />
<code>gm2_get_font_style()</code> *New<br />
<code>gm2_get_font_stretch()</code> *New<br />
<code>gm2_get_font_locale()</code> *New<br />
<code>gm2_get_font_min_aa_h_size()</code> *New<br />
<code>gm2_get_font_min_aa_v_size()</code> *New</h2>

> v1: `gm_get_font()`

Get the current font.

<h2><code>gm2_set_direction(direction)</code> *New<br />
<code>gm2_set_text_direction(text_direction)</code> *New<br />
<code>gm2_set_par_direction(par_direction)</code> *New</h2>

Set the text direction or the paragraph direction.

<h2><code>gm2_get_text_direction()</code> *New<br />
<code>gm2_get_par_direction()</code> *New</h2>

Get the text direction or the paragraph direction.

<h2><code>gm2_set_alignment(alignment)</code><br />
<code>gm2_set_text_alignment(text_alignment)</code><br />
<code>gm2_set_par_alignment(par_alignment)</code></h2>

> v1:
> 
> - `gm_set_align3()`
> - `gm_set_align()`
> - `gm_set_halign()`
> - `gm_set_valign()`
> - `gm_set_justified()`
> 

Set the text alignment or the paragraph alignment.

**Dropped**: `justified` is no longer independent of `left`/`center`/`right`.

<h2><code>gm2_get_text_alignment()</code><br />
<code>gm2_get_par_alignment()</code></h2>

> v1:
> 
> - `gm_get_halign()`
> - `gm_get_valign()`
> - `gm_is_justified()`
> 

Get the text alignment or the paragraph alignment.

<h2><code>gm2_set_max_width(max_width)</code></h2>

> v1: `gm_set_max_line_length()`

Set the maximum width of the text.

<h2><code>gm2_get_max_width()</code></h2>

> v1: `gm_get_max_line_length()`

Get the maximum width of the text.

<h2><code>gm2_set_max_height(max_height)</code> *New</h2>

Set the maximum height of the text.

<h2><code>gm2_get_max_height()</code> *New</h2>

Get the maximum height of the text.

<h2><code>gm2_set_word_wrapping(word_wrapping)</code> *New</h2>

Set the word wrapping of the text.

<h2><code>gm2_get_word_wrapping()</code> *New</h2>

Get the word wrapping of the text.

<h2><code>gm2_set_trimming(trimming)</code> *New</h2>

Set the text trimming.

<h2><code>gm2_get_trimming()</code> *New</h2>

Get the text trimming.

<h2><code>gm2_set_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_fixed_line_spacing(line_height, baseline)</code><br />
<code>gm2_set_line_height(line_height)</code><br />
<code>gm2_set_baseline(baseline)</code></h2>

> v1:
> 
> - `gm_set_line_height()`
> - `gm_set_paragraph_spacing()`
> 

Set the line spacing of the text.

**New**: Proportional line height.

<h2><code>gm2_get_line_spacing_type()</code><br />
<code>gm2_get_line_height()</code><br />
<code>gm2_get_baseline()</code></h2>

> v1:
> 
> - `gm_get_line_height()`
> - `gm_get_paragraph_spacing()`
> 

Get the line spacing of the text.

<h2><code>gm2_set_tab_spacing(tab_spacing)</code> *New</h2>

Set the tab spacing of the text.

<h2><code>gm2_get_tab_spacing()</code> *New</h2>

Get the tab spacing of the text.

<h2><code>gm2_set_letter_spacing(letter_spacing)</code></h2>

> v1: `gm_set_letter_spacing()`

Set the letter spacing of the text.

<h2><code>gm2_get_letter_spacing()</code></h2>

> v1: `gm_get_letter_spacing()`

Get the letter spacing of the text.

<h2><code>gm_free()</code><br />
<code>gm_clear()</code></h2>

Free a font or all fonts.

**Removed**: Glyphs are extracted on demand, making manual memory management no longer necessary.

<h2><code>gm_set_word_spacing()</code><br />
<code>gm_get_word_spacing()</code></h2>

Set or get the word spacing of the text.

**Removed**: Not useful.
