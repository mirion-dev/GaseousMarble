import math
import os
import struct

from fontTools.ttLib import TTFont
from PIL import Image, ImageDraw, ImageFont


def generate_font(
    font_path: str | list[str],
    sprite_path: str,
    *,
    font_size=16,
    charset: str | None = None,
    dense=False,
    smoothing=True,
    fill='white',
    stroke_width=0,
    stroke_fill='black',
    shadow_offset=0,
    shadow_fill='black'
):
    if font_size <= 0:
        raise ValueError('The `font_size` should be positive.')
    if charset is not None and not charset:
        raise ValueError('The `charset` should be non-empty.')
    if stroke_width < 0:
        raise ValueError('The `stroke_width` should be non-negative.')
    if shadow_offset < 0:
        raise ValueError('The `shadow_offset` should be non-negative.')

    if isinstance(font_path, str):
        font_path = [font_path]

    # assign fonts to every character in the charset
    chars_map = dict[str, set[str]]()
    assigned_chars = set[str]()
    for path in font_path:
        code_points = TTFont(path).getBestCmap()
        if code_points is None:
            continue

        chars = set(filter(str.isprintable, map(chr, code_points.keys())))
        chars_map[path] = chars - assigned_chars
        assigned_chars |= chars_map[path]

    if charset is not None:
        needed_chars = set(filter(str.isprintable, charset))
        for (path, chars) in chars_map.items():
            chars &= needed_chars
            if chars:
                needed_chars -= chars
            else:
                chars_map.pop(path)

        if needed_chars:
            raise ValueError(f'Unable to find a suitable font for following characters: {needed_chars}')

    chars_map = {path: ''.join(sorted(chars)) for (path, chars) in chars_map.items()}
    fonts = [ImageFont.truetype(path, font_size) for path in chars_map.keys()]

    # calculate the sprite size when in a single line
    image0 = Image.new('RGBA', (1, 1))
    draw0 = ImageDraw.Draw(image0)
    if not smoothing:
        draw0.fontmode = '1'

    glyph_spacing = 0 if dense else 1
    line_width = 0
    min_top = 0
    max_bottom = 0
    max_glyph_width = 0
    for (font, chars) in zip(fonts, chars_map.values()):
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = map(round, draw0.textbbox((0, 0), ch, stroke_width=stroke_width))
            w = r - l + shadow_offset
            line_width += w + glyph_spacing
            min_top = min(min_top, t)
            max_bottom = max(max_bottom, b + shadow_offset)
            max_glyph_width = max(max_glyph_width, w)

    line_width -= glyph_spacing
    line_height = max_bottom - min_top + glyph_spacing

    # calculate the sprite size to arrange glyphs into a roughly square
    sprite_width = max(math.ceil((line_height + math.sqrt(line_height * (line_height + 4 * line_width))) / 2), max_glyph_width)  # x == h * (l / x + 1)
    line_width = 0
    line_count = 1
    for (font, chars) in zip(fonts, chars_map.values()):
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = map(round, draw0.textbbox((0, 0), ch, stroke_width=stroke_width))
            w = r - l + shadow_offset
            if line_width + w > sprite_width:
                line_width = 0
                line_count += 1
            line_width += w + glyph_spacing

    sprite_height = line_height * line_count - glyph_spacing

    # generates the font sprite and glyph data
    image = Image.new('RGBA', (sprite_width, sprite_height))
    draw = ImageDraw.Draw(image)
    if not smoothing:
        draw.fontmode = '1'

    data_path = os.path.splitext(sprite_path)[0] + '.gly'
    os.makedirs(os.path.dirname(data_path), exist_ok=True)
    with open(data_path, 'wb+') as file:
        file.write(b'GLY\x01\x00\x00' + struct.pack('Hh', line_height - glyph_spacing, min_top))

        x = 0
        y = 0
        for (font, chars) in zip(fonts, chars_map.values()):
            draw.font = font
            for ch in chars:
                (raw_l, raw_t, raw_r, raw_b) = map(round, draw.textbbox((0, 0), ch))
                (l, t, r, b) = map(round, draw.textbbox((0, 0), ch, stroke_width=stroke_width))
                w = r - l + shadow_offset
                a = w + round(draw.textlength(ch) - (raw_r - raw_l))
                if x + w > sprite_width:
                    x = 0
                    y += line_height
                file.write(struct.pack('IHHHhh', ord(ch), x, y, w, a, raw_l))

                draw_x = x - l
                draw_y = y - min_top
                # draw.rectangle(((draw_x + l - 1, draw_y + t - 1), (draw_x + r + shadow_offset, draw_y + b + shadow_offset)), outline='red')
                if shadow_offset != 0:
                    draw.text((draw_x + shadow_offset, draw_y + shadow_offset), ch, fill=shadow_fill, stroke_width=stroke_width, stroke_fill=shadow_fill)
                draw.text((draw_x, draw_y), ch, fill, stroke_width=stroke_width, stroke_fill=stroke_fill)
                draw.text((draw_x, draw_y), ch, fill)  # make glyphs more clear

                x += w + glyph_spacing

    image.save(sprite_path)
