from typing import cast
from fontTools.ttLib import TTFont
from PIL import Image, ImageFont, ImageDraw
import os
import math
import struct


def generate_font(
    font_path: str | list[str], sprite_path: str, *,
    font_size=16, charset: str | None = None, dense=False, smoothing=True, fill='white', stroke_width=0, stroke_fill='black'
):
    if font_size <= 0:
        raise Exception('The `font_size` should be positive.')
    if charset != None and len(charset) == 0:
        raise Exception('The `charset` should be non-empty.')
    if stroke_width < 0:
        raise Exception('The `stroke_width` should be non-negative.')

    if isinstance(font_path, str):
        font_path = [font_path]

    '''
    assign font to every character in the charset
    '''
    chars_map = dict[str, set[str]]()
    assigned_chars = set[str]()
    for path in font_path:
        code_points = cast(dict[int, str] | None, TTFont(path).getBestCmap())
        if code_points == None:
            continue

        chars = {chr(i) for i in code_points.keys() if chr(i).isprintable()}
        chars_map[path] = chars - assigned_chars
        assigned_chars |= chars_map[path]

    if charset != None:
        needed_chars = set(filter(str.isprintable, charset))
        for (path, chars) in chars_map.items():
            chars &= needed_chars
            if len(chars) == 0:
                chars_map.pop(path)
            else:
                needed_chars -= chars

        if len(needed_chars) != 0:
            raise Exception(f'Unable to find a suitable font for following characters: {list(needed_chars)}')

    chars_map = {path: ''.join(sorted(chars)) for (path, chars) in chars_map.items()}
    fonts = [ImageFont.truetype(path, font_size) for path in chars_map.keys()]

    '''
    calculate the sprite size when in a single line
    '''
    image0 = Image.new('RGBA', (1, 1))
    draw0 = ImageDraw.Draw(image0)
    if not smoothing:
        draw0.fontmode = '1'

    line_length = 0
    glyph_spacing = 0 if dense else 1
    min_top = 0
    max_bottom = 0
    for (font, chars) in zip(fonts, chars_map.values()):
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = draw0.textbbox((0, 0), ch, stroke_width=stroke_width)
            w = r - l
            line_length += w + glyph_spacing
            min_top = min(min_top, t)
            max_bottom = max(max_bottom, b)

    line_length = int(line_length) - glyph_spacing
    line_height = int(max_bottom - min_top) + glyph_spacing

    '''
    calculate the sprite size to arrange glyphs into a roughly square
    '''
    max_line_length = line_length
    line_count = 1
    if max_line_length > 1024:
        # x == h * (l / x + 1)
        max_line_length = int((line_height + math.sqrt(line_height * (line_height + 4 * line_length))) / 2)
        line_length = 0
        for (font, chars) in zip(fonts, chars_map.values()):
            draw0.font = font
            for ch in chars:
                (l, t, r, b) = draw0.textbbox((0, 0), ch, stroke_width=stroke_width)
                w = r - l
                if line_length + w > max_line_length:
                    line_length = 0
                    line_count += 1
                line_length += w + glyph_spacing

    '''
    generates the font sprite and glyph data
    '''
    image = Image.new('RGBA', (max_line_length, line_height * line_count - glyph_spacing))
    draw = ImageDraw.Draw(image)
    if not smoothing:
        draw.fontmode = '1'

    data_path = os.path.splitext(sprite_path)[0] + '.gly'
    os.makedirs(os.path.dirname(data_path), exist_ok=True)
    with open(data_path, 'wb+') as file:
        file.write(b'GLY\x00\x12\x00' + struct.pack('Hh', line_height - glyph_spacing, int(min_top)))

        x = 0
        y = -min_top
        for (font, chars) in zip(fonts, chars_map.values()):
            draw.font = font
            for ch in chars:
                (l, t, r, b) = draw.textbbox((0, 0), ch, stroke_width=stroke_width)
                a = draw.textlength(ch)
                w = r - l
                if x + w > max_line_length:
                    x = 0
                    y += line_height
                file.write(struct.pack('IHHHHh', ord(ch), int(x), int(y), int(w), int(a), int(l)))

                pos = (int(x - l), int(y))
                # draw.rectangle(((x - 1, y + t - 1), (x + w, y + b)), outline='red')
                draw.text(pos, ch, fill, stroke_width=stroke_width, stroke_fill=stroke_fill)
                draw.text(pos, ch, fill)  # make glyphs more clear

                x += w + glyph_spacing

    image.save(sprite_path)
