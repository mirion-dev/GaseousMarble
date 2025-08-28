from typing import cast
from fontTools.ttLib import TTFont
from PIL import Image, ImageFont, ImageDraw
import os
import math
import struct


def generate_font(font_path: str | list[str], sprite_path: str, *, font_size=16, charset: str | None = None, dense=False, smoothing=True, fill='white', stroke_width=0, stroke_fill='black', shadow_offset=0, shadow_fill='black'):
    if font_size <= 0:
        raise Exception('The `font_size` should be positive.')
    if charset != None and len(charset) == 0:
        raise Exception('The `charset` should be non-empty.')
    if stroke_width < 0:
        raise Exception('The `stroke_width` should be non-negative.')
    if shadow_offset < 0:
        raise Exception('The `shadow_offset` should be non-negative.')

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
    min_top = 0
    max_bottom = 0
    for (font, chars) in zip(fonts, chars_map.values()):
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = map(round, draw0.textbbox((0, 0), ch, stroke_width=stroke_width))
            w = r - l + shadow_offset
            line_length += w + dense
            min_top = min(min_top, t)
            max_bottom = max(max_bottom, b + shadow_offset)

    line_length = line_length - dense
    line_height = max_bottom - min_top + dense

    '''
    calculate the sprite size to arrange glyphs into a roughly square
    '''
    max_line_length = line_length
    line_count = 1
    if max_line_length > 1024:
        # x == h * (l / x + 1)
        max_line_length = math.ceil((line_height + math.sqrt(line_height * (line_height + 4 * line_length))) / 2)
        line_length = 0
        for (font, chars) in zip(fonts, chars_map.values()):
            draw0.font = font
            for ch in chars:
                (l, t, r, b) = map(round, draw0.textbbox((0, 0), ch, stroke_width=stroke_width))
                w = r - l + shadow_offset
                if line_length + w > max_line_length:
                    line_length = 0
                    line_count += 1
                line_length += w + dense

    '''
    generates the font sprite and glyph data
    '''
    image = Image.new('RGBA', (max_line_length, line_height * line_count - dense))
    draw = ImageDraw.Draw(image)
    if not smoothing:
        draw.fontmode = '1'

    data_path = os.path.splitext(sprite_path)[0] + '.gly'
    os.makedirs(os.path.dirname(data_path), exist_ok=True)
    with open(data_path, 'wb+') as file:
        file.write(b'GLY\x01\x00\x00' + struct.pack('Hh', line_height - dense, min_top))

        x = 0
        y = 0
        for (font, chars) in zip(fonts, chars_map.values()):
            draw.font = font
            for ch in chars:
                (raw_l, raw_t, raw_r, raw_b) = map(round, draw.textbbox((0, 0), ch))
                (l, t, r, b) = map(round, draw.textbbox((0, 0), ch, stroke_width=stroke_width))
                w = r - l + shadow_offset
                a = w + round(draw.textlength(ch) - (raw_r - raw_l))
                if x + w > max_line_length:
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

                x += w + dense

    image.save(sprite_path)
