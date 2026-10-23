"""Render remote layout states using the original VLW fonts and JPEG icons.

Host layout review only: does not measure LCD speed, SPI or battery consumption.
"""
from pathlib import Path
import struct
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'artifacts' / 'remote-ui'
OUT.mkdir(parents=True, exist_ok=True)
fonts = {}
for name in ('Noway_Light18', 'Noway_Medium24', 'Noway_Medium60'):
    data = (ROOT / 'shared/assets/fonts' / (name + '.vlw')).read_bytes()
    count, _, size, _, ascent, _ = struct.unpack_from('>6i', data)
    offset = 24 + count * 28
    glyphs = {}
    for i in range(count):
        code, height, width, advance, dy, dx, _ = struct.unpack_from('>7i', data, 24+i*28)
        bitmap = Image.frombytes('L', (width, height), data[offset:offset+width*height]) if width*height else None
        glyphs[chr(code)] = (bitmap, advance, dy, dx)
        offset += width*height
    fonts[name] = (size, ascent, glyphs)

def width(name, text):
    size, _, glyphs = fonts[name]
    return sum(7 if c=='°' else glyphs.get(c, (None, size//3, 0, 0))[1] for c in text)

def text(im, name, value, x, y, color='#212021'):
    size, ascent, glyphs = fonts[name]
    for c in value:
        if c=='°':
            ImageDraw.Draw(im).ellipse((x+1,y+1,x+5,y+5),outline=color)
            x+=7
            continue
        bitmap, advance, dy, dx = glyphs.get(c, (None, size//3, 0, 0))
        if bitmap:
            px, py = x+dx, y+ascent-dy
            assert 0 <= px and px+bitmap.width <= 240, (name, value, px)
            assert 0 <= py and py+bitmap.height <= 320, (name, value, py)
            im.paste(color, (px, py), bitmap)
        x += advance

def center(im, name, value, y, color='#212021'):
    text(im, name, value, (240-width(name, value))//2, y, color)

def icon(im, name, x, y):
    im.paste(Image.open(ROOT/'shared/assets/icons'/name).convert('RGB'), (x, y))

def render(label, link, room='20.3', setpoint='21.0', heating=True, battery='76%'):
    im=Image.new('RGB',(240,320),'white');d=ImageDraw.Draw(im)
    grey='#6b6d6b';line='#e7e3e7';color='#298e5a' if link==1 else '#bd6100' if link>=2 else grey
    d.line((16,44,223,44),fill=line);d.line((16,275,223,275),fill=line)
    d.ellipse((16,19,22,25),fill=color)
    text(im,'Noway_Light18',['Verbinden...','Verbonden','Oude meting','Geen verbinding','Radiofout'][link],29,14,color)
    text(im,'Noway_Light18',battery,222-width('Noway_Light18',battery),14,grey)
    icon(im,'iconTemperature.jpg',16,55);text(im,'Noway_Light18','Kamertemperatuur',54,65,grey)
    fg='#212021' if link==1 else grey
    x=(240-width('Noway_Medium60',room)-8-width('Noway_Medium24','°C'))//2
    text(im,'Noway_Medium60',room,x,99,fg)
    text(im,'Noway_Medium24','°C',x+width('Noway_Medium60',room)+8,123,fg)
    d.rounded_rectangle((16,179,223,237),radius=10,outline=line)
    icon(im,'iconSetpoint.jpg',24,192);text(im,'Noway_Light18','Ingesteld',64,186,grey)
    value=setpoint+' °C';text(im,'Noway_Medium24',value,212-width('Noway_Medium24',value),208,fg)
    if link==1:
        icon(im,'iconFlame1.jpg' if heating else 'iconFlame0.jpg',20,241)
        text(im,'Noway_Light18','Verwarming actief' if heating else 'Verwarming in rust',60,249,'#bd6100' if heating else grey)
    else: center(im,'Noway_Light18','Wachten op actuele status',249,grey)
    # Small diagnostic footer approximates the GFX 6x8 built-in font.
    for value,y in [('Raak aan om wakker te blijven',285),('3.0.0-alpha.1',303)]:
        d.text(((240-d.textlength(value))//2,y),value,fill=grey)
    im.save(OUT/(label+'.png'))
    return im

states=[render('connected',1),render('searching',0,'--.-','--.-'),render('stale',2)]
# Exercise unusual values and every connection label, including low battery.
for link in range(5):
    render('check-'+str(link),link,'-10.0','100.0',False,'100%')
render('check-negative-bound',2,'-327.7','-327.7',False,'--%')
render('check-positive-bound',1,'327.7','327.7',False,'1%')
canvas=Image.new('RGB',(3*240+4*16,352),'#eeece7')
for i,im in enumerate(states):canvas.paste(im,(16+i*256,16))
canvas.resize((1600,704)).save(OUT/'comparison.png')
print('Rendered three review states and five boundary cases; all VLW glyphs fit the screen.')
