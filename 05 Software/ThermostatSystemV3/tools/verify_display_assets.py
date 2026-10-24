"""Check generated flash artwork against the source JPEGs, without hardware."""
from pathlib import Path
import re
from display_asset_sources import selected_assets,opaque_image

root = Path(__file__).resolve().parents[1]
source = (root / 'shared/assets/include/display_assets.h').read_text()
entries = re.findall(r'\{"(/[^\"]+)",(\d+),(\d+),image(\d+)\}', source)
files=dict(selected_assets(root))
assert len(entries) == len(files)
assert len({name for name,_,_,_ in entries})==len(entries)
for name, width, height, index in entries:
    im = opaque_image(files[name[1:]])
    width, height = int(width), int(height)
    assert im.size == (width, height) and width <= 320
    raw = re.search(r'image'+index+r'\[\] PROGMEM = \{(.*?)\};', source, re.S).group(1)
    values = [int(v,16) for v in re.findall(r'0x[0-9a-f]{4}', raw)]
    assert len(values) == width*height
    for value, (r,g,b) in zip(values, im.getdata()):
        assert 0 <= r-((value>>11)<<3) <= 7
        assert 0 <= g-(((value>>5)&63)<<2) <= 3
        assert 0 <= b-((value&31)<<3) <= 7
print(f'{len(files)} assets verified: PNG precedence, white alpha composition, dimensions, pixels, RGB565 and buffers')
menu=(root/'remote-cyd/include/remote_menu.h').read_text()
names=set(re.findall(r'"(/[^"\n]+\.(?:jpg|png))"',menu))
missing=[name for name in names if '%' not in name and not (root/'shared/assets/icons'/name[1:]).is_file()]
assert not missing,missing
print(f'{len(names)} named firmware UI assets exist')
