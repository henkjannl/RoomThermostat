"""Check generated flash artwork against the source JPEGs, without hardware."""
from pathlib import Path
import re
from PIL import Image

root = Path(__file__).resolve().parents[1]
source = (root / 'src/display_assets.h').read_text()
entries = re.findall(r'\{"(/[^\"]+)",(\d+),(\d+),image(\d+)\}', source)
assert len(entries) == 51
for name, width, height, index in entries:
    im = Image.open(root / 'data' / name[1:]).convert('RGB')
    width, height = int(width), int(height)
    assert im.size == (width, height) and width <= 320
    raw = re.search(r'image'+index+r'\[\] PROGMEM = \{(.*?)\};', source, re.S).group(1)
    values = [int(v,16) for v in re.findall(r'0x[0-9a-f]{4}', raw)]
    assert len(values) == width*height
    for value, (r,g,b) in zip(values, im.getdata()):
        assert 0 <= r-((value>>11)<<3) <= 7
        assert 0 <= g-(((value>>5)&63)<<2) <= 3
        assert 0 <= b-((value&31)<<3) <= 7
print('51 assets verified: dimensions, pixel counts, RGB565 colors and row-buffer bounds')
