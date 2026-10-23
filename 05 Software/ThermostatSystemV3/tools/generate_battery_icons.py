"""Generate small native bitmap charge indicators for the remote design."""
from pathlib import Path
from PIL import Image, ImageDraw

root=Path(__file__).resolve().parents[1]
out=root/'shared/assets/icons'
for percent in (0,25,50,75,100):
    scale=4
    im=Image.new('RGB',(32*scale,32*scale),'white')
    d=ImageDraw.Draw(im)
    d.rounded_rectangle((2*scale,9*scale,27*scale,23*scale),radius=2*scale,outline='#52534f',width=scale)
    d.rectangle((28*scale,13*scale,30*scale,19*scale),fill='#52534f')
    if percent:
        d.rectangle((5*scale,12*scale,(5+19*percent/100)*scale,20*scale),fill='#b04b30' if percent<=25 else '#637961')
    im.resize((32,32),Image.Resampling.LANCZOS).save(out/f'battery-{percent}.png')
print('Generated five 32x32 battery indicators; no charging state inferred.')
