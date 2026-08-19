import shutil
from pathlib import Path

from utils import font

FONT_PATH = Path('font.otf')
TEST_DIR = Path('../projects/test.gm82/gm_fonts')
EXAMPLE_DIR = Path('../projects/example.gm82/gm_fonts')

CHARSET = ''.join(map(chr, range(128))) + '''
It was the best of times, 这是一个最坏的时代, それは智慧の時代であり, 어리석음의 시대이기도 했다.
'''

font.generate_font(FONT_PATH, TEST_DIR / 'default.png', font_size=18, charset=CHARSET)
shutil.copytree(TEST_DIR, EXAMPLE_DIR, dirs_exist_ok=True)
