import math
import struct
from os import PathLike
from pathlib import Path

from fontTools import ttLib
from PIL import Image, ImageDraw, ImageFont


def generate_font(
    font_path: str | PathLike[str] | list[str | PathLike[str]],
    sprite_path: str | PathLike[str],
    *,
    font_size: int = 16,
    charset: str | None = None,
    dense: bool = False,
    smoothing: bool = True,
    fill: str = 'white',
    stroke_width: int = 0,
    stroke_fill: str = 'black',
    shadow_offset: int = 0,
    shadow_fill: str = 'black',
    debug: bool = False
):
    if font_size <= 0:
        raise ValueError('The `font_size` should be positive.')
    if stroke_width < 0:
        raise ValueError('The `stroke_width` should be non-negative.')
    if shadow_offset < 0:
        raise ValueError('The `shadow_offset` should be non-negative.')

    if not isinstance(font_path, list):
        font_path = [font_path]

    font_path_ = [Path(p) for p in font_path]
    sprite_path = Path(sprite_path)

    # Assign fonts to every character in the charset
    chars_map: dict[Path, set[str]] = {}
    assigned_chars: set[str] = set()
    for path in font_path_:
        with ttLib.TTFont(path, fontNumber=0) as font:
            code_points = font.getBestCmap()
        if code_points is None:
            continue

        chars = {c for c in map(chr, code_points.keys()) if c.isprintable()}
        chars_map[path] = chars - assigned_chars
        assigned_chars |= chars_map[path]

    if charset is not None:
        used_chars = {c for c in charset if c.isprintable()}
        if not used_chars:
            raise ValueError('The `charset` should be non-empty.')

        for (path, chars) in list(chars_map.items()):
            chars &= used_chars
            if chars:
                used_chars -= chars
            else:
                chars_map.pop(path)

        if used_chars:
            raise ValueError(f'Unable to find a suitable font for following characters: {used_chars}')

    charset_map = [(ImageFont.truetype(path, font_size), ''.join(sorted(chars))) for (path, chars) in chars_map.items()]

    # Calculate the sprite size when in a single line
    def bbox(draw: ImageDraw.ImageDraw, ch: str, stroke_width: int = stroke_width, shadow_offset: int = shadow_offset):
        (l, t, r, b) = map(round, draw.textbbox((0, 0), ch, stroke_width=stroke_width))
        r += shadow_offset
        b += shadow_offset
        return (l, t, r, b)

    image0 = Image.new('RGBA', (1, 1))
    draw0 = ImageDraw.Draw(image0)
    if not smoothing:
        draw0.fontmode = '1'

    glyph_spacing = 0 if dense else 1
    line_width = 0
    min_glyph_top = 0
    max_glyph_bottom = 0
    max_glyph_width = 0
    for (font, chars) in charset_map:
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = bbox(draw0, ch)
            w = r - l
            line_width += w + glyph_spacing
            min_glyph_top = min(min_glyph_top, t)
            max_glyph_bottom = max(max_glyph_bottom, b)
            max_glyph_width = max(max_glyph_width, w)

    line_width -= glyph_spacing
    line_height = max_glyph_bottom - min_glyph_top

    # Calculate the sprite size to arrange glyphs into a roughly square
    sprite_width = max(math.ceil((line_height + math.sqrt(line_height ** 2 + 4 * line_width * (line_height + glyph_spacing))) / 2), max_glyph_width)  # (w / x + 1) * h + w / x * s == x
    line_width = 0
    line_num = 1
    max_line_width = 0
    for (font, chars) in charset_map:
        draw0.font = font
        for ch in chars:
            (l, t, r, b) = bbox(draw0, ch)
            w = r - l
            if line_width + w > sprite_width:
                max_line_width = max(max_line_width, line_width - glyph_spacing)
                line_width = 0
                line_num += 1
            line_width += w + glyph_spacing

    max_line_width = max(max_line_width, line_width - glyph_spacing)
    sprite_width = max_line_width
    sprite_height = (line_height + glyph_spacing) * line_num - glyph_spacing

    # Generate the font sprite and the glyph data
    image = Image.new('RGBA', (sprite_width, sprite_height))
    draw = ImageDraw.Draw(image)
    if not smoothing:
        draw.fontmode = '1'

    data_path = sprite_path.with_suffix('.gly')
    data_path.parent.mkdir(parents=True, exist_ok=True)
    with open(data_path, 'wb+') as data:
        data.write(b'GLY\x01\x01\x00' + struct.pack('HhI', line_height, min_glyph_top, sum(map(len, chars_map.values()))))

        x = 0
        y = 0
        for (font, chars) in charset_map:
            draw.font = font
            for ch in chars:
                (raw_l, _, raw_r, _) = bbox(draw, ch, 0, 0)
                raw_w = raw_r - raw_l
                raw_a = round(draw.textlength(ch))
                (l, t, r, b) = bbox(draw, ch)
                w = r - l
                if x + w > sprite_width:
                    x = 0
                    y += line_height + glyph_spacing
                data.write(struct.pack('IHHHhh', ord(ch), x, y, raw_w, raw_a, raw_l))

                draw_x = x - l
                draw_y = y - min_glyph_top
                if debug:
                    draw.rectangle(((draw_x + l - 1, draw_y + t - 1), (draw_x + r, draw_y + b)), outline='#f007', fill='#0f07')
                if shadow_offset != 0:
                    draw.text((draw_x + shadow_offset, draw_y + shadow_offset), ch, fill=shadow_fill, stroke_width=stroke_width, stroke_fill=shadow_fill)
                draw.text((draw_x, draw_y), ch, fill, stroke_width=stroke_width, stroke_fill=stroke_fill)
                draw.text((draw_x, draw_y), ch, fill)  # Make glyphs more clear

                x += w + glyph_spacing

    image.save(sprite_path, 'PNG')
