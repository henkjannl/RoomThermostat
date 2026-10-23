"""Generate code-native bitmap controls for the small remote display."""
from pathlib import Path
from PIL import Image, ImageDraw

out=Path(__file__).resolve().parents[1]/'shared/assets/icons'
scale=4
def make(name,paint):
    im=Image.new('RGB',(128,128),'white');d=ImageDraw.Draw(im)
    paint(d)
    im.resize((32,32),Image.Resampling.LANCZOS).save(out/f'remote-{name}.png')
def line(d,points,fill='#555650',width=3):
    d.line([(x*scale,y*scale) for x,y in points],fill=fill,width=width*scale,joint='curve')
def circle_control(sign,color):
    def draw(d):
        d.ellipse((4,4,124,124),fill=color)
        line(d,[(9,16),(23,16)],'white')
        if sign=='+':line(d,[(16,9),(16,23)],'white')
    return draw
make('minus',circle_control('-','#347bb7'))
make('plus',circle_control('+','#c44c44'))
make('menu',lambda d:[line(d,[(7,y),(25,y)],width=2) for y in (9,16,23)])
make('previous',lambda d:d.polygon([(21*4,7*4),(10*4,16*4),(21*4,25*4)],fill='#555650'))
make('next',lambda d:d.polygon([(11*4,7*4),(22*4,16*4),(11*4,25*4)],fill='#555650'))
def bed(d):
    line(d,[(5,8),(5,26)],width=2);line(d,[(27,16),(27,26)],width=2)
    d.rounded_rectangle((6*4,15*4,26*4,22*4),radius=4,fill='#777b7c')
    d.rounded_rectangle((7*4,11*4,13*4,15*4),radius=4,fill='#b6c7cc')
make('sleep',bed)
def alarm(d):
    d.ellipse((8*4,8*4,24*4,24*4),outline='#555650',width=2*4)
    line(d,[(16,11),(16,16),(21,16)],width=2)
    line(d,[(7,7),(11,4)],width=2);line(d,[(21,4),(25,7)],width=2)
    line(d,[(11,24),(8,28)],width=2);line(d,[(21,24),(24,28)],width=2)
make('wake',alarm)
def house_arrow(inward):
    def draw(d):
        d.polygon([(4*4,13*4),(13*4,5*4),(22*4,13*4)],fill='#b0987a')
        d.rectangle((7*4,13*4,20*4,27*4),outline='#777b7c',width=2*4)
        line(d,[(15,20),(29,20)],width=2)
        tip=15 if inward else 29;tail=tip+4 if inward else tip-4
        line(d,[(tail,16),(tip,20),(tail,24)],width=2)
    return draw
make('leave',house_arrow(False));make('arrive',house_arrow(True))
print('Generated nine 32x32 controls: coloured minus/plus, menu, arrows, alarm, bed, leave and arrive.')
