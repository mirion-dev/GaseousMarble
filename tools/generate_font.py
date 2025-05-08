from fontTools.ttLib import TTFont
from PIL import Image, ImageFont, ImageDraw
import os
import math
import struct


def generate_font(
    font_path_list: str | list[str], sprite_path: str, *,
    font_size=16, char_list: str | None = None, dense=False, smoothing=True, fill='white', stroke_width=0, stroke_fill='black'
):
    if font_size <= 0:
        raise Exception('The font size should be positive.')
    if stroke_width < 0:
        raise Exception('The stroke width should be non-negative.')

    if isinstance(font_path_list, str):
        font_path_list = [font_path_list]

    font_path_list = list(map(os.path.abspath, font_path_list))
    sprite_path = os.path.abspath(sprite_path)
    glyph_path = os.path.splitext(sprite_path)[0] + '.gly'

    '''
    counts used code points
    '''
    cp_map = {}
    cp_list = set()
    for path in font_path_list:
        cp = set()
        with TTFont(path) as font:
            for table in font['cmap'].tables:
                cp |= table.cmap.keys()
        cp -= cp_list

        cp_map[path] = set(filter(lambda x: chr(x).isprintable(), cp))
        cp_list |= cp_map[path]

    if char_list != None:
        cp_char_list = set(map(ord, filter(str.isprintable, char_list)))
        for cp in cp_map.values():
            cp &= cp_char_list
            cp_char_list -= cp

        if len(cp_char_list) != 0:
            raise Exception(f'Unable to find a suitable font for following characters: {list(map(chr, cp_char_list))}')

    code_points = {path: sorted(cp) for (path, cp) in cp_map.items()}
    font_list = [ImageFont.truetype(path, font_size) for path in code_points]

    '''
    calculates the line height and min top
    '''
    image0 = Image.new('RGBA', (1, 1))
    draw0 = ImageDraw.Draw(image0)
    if not smoothing:
        draw0.fontmode = '1'

    line_length = 0
    glyph_spacing = 0 if dense else 1
    min_top = 0
    max_bottom = 0
    for (font, cp) in zip(font_list, code_points.values()):
        draw0.font = font
        for i in cp:
            (l, t, r, b) = draw0.textbbox((0, 0), chr(i), stroke_width=stroke_width)
            w = r - l
            line_length += w + glyph_spacing
            min_top = min(min_top, t)
            max_bottom = max(max_bottom, b)

    line_length -= glyph_spacing
    line_height = max_bottom - min_top + glyph_spacing

    '''
    calculates the max line length
    '''
    max_line_length = line_length
    line_count = 1
    if max_line_length > 1024:
        # x == h * (l / x + 1)
        max_line_length = math.ceil((line_height + math.sqrt(line_height * (line_height + 4 * line_length))) / 2)
        line_length = 0
        for (font, cp) in zip(font_list, code_points.values()):
            draw0.font = font
            for i in cp:
                (l, t, r, b) = draw0.textbbox((0, 0), chr(i), stroke_width=stroke_width)
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

    os.makedirs(os.path.dirname(glyph_path), exist_ok=True)
    with open(glyph_path, 'wb+') as file:
        file.write(b'GLY\0' + struct.pack('Hh', line_height, min_top))

        x = 0
        y = -min_top
        for (font, cp) in zip(font_list, code_points.values()):
            draw.font = font
            for i in cp:
                (l, t, r, b) = draw.textbbox((0, 0), chr(i), stroke_width=stroke_width)
                w = r - l
                if x + w > max_line_length:
                    x = 0
                    y += line_height
                file.write(struct.pack('IHHHh', i, x, y, w, l))

                pos = (x - l, y)
                # draw.rectangle(((x - 1, y + t - 1), (x + w, y + b)), outline='red')
                draw.text(pos, chr(i), fill, stroke_width=stroke_width, stroke_fill=stroke_fill)
                draw.text(pos, chr(i), fill)

                x += w + glyph_spacing

    image.save(sprite_path)
