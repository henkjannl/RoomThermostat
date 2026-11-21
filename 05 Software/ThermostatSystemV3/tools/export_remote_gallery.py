"""Export native firmware framebuffer examples; run check_remote_native.cmd first."""
from pathlib import Path
from PIL import Image, ImageDraw
import html
root=Path(__file__).resolve().parents[1]
out=root/'artifacts/remote-ui'
groups=[('Menus', [('home','Hoofdpagina'),('menu','Menu'),('today','Vandaag'),('tomorrow','Morgen'),('period','Meer dagen'),('multiple','Dagtype meer dagen'),('settings','Instellingen'),('week','Weekschema'),('weekday','Dagtype weekschema')]),('Instellingen', [('times','Tijden'),('home-times','Tijden thuiswerken'),('office-times','Tijden kantoor'),('weekend-times','Tijden weekend'),('temperatures','Temperaturen'),('more','Meer instellingen'),('status','Verbindingsstatus'),('service','Service')]),('Bewerken en meldingen', [('edit-time','Tijd aanpassen'),('edit-comfort','Comforttemperatuur'),('edit-night','Nachttemperatuur'),('edit-offset','Sensorcorrectie'),('edit-duration','Aantal dagen'),('edit-unlimited','Onbeperkt'),('confirm-day','Dagtype bevestigen'),('confirm-auto','Automatisch bevestigen'),('confirm-restart','Herstart bevestigen'),('connecting','Verbinden'),('disconnected','Geen verbinding'),('radio-error','Radiofout')])]
parts=['<!doctype html><html lang="nl"><meta charset="utf-8"><title>Remote schermvoorbeelden</title><style>body{font:16px system-ui;background:#eee;color:#222;margin:24px}section{display:flex;flex-wrap:wrap;gap:20px}figure{margin:0 0 20px;background:white;padding:12px;border-radius:8px}figcaption{margin-bottom:10px}img{width:240px;height:320px}</style><h1>Remote schermvoorbeelden</h1><p>Getekend door de firmware op de computer, met voorbeeldwaarden.</p>']
for idx,(title,items) in enumerate(groups,1):
    cols=4;rows=(len(items)+cols-1)//cols
    sheet=Image.new('RGB',(cols*260,rows*360),'#eeeeee');d=ImageDraw.Draw(sheet)
    parts.append('<h2>'+title+'</h2><section>')
    for n,(name,label) in enumerate(items):
        im=Image.open(out/('firmware-'+name+'.ppm'));im.save(out/('example-'+name+'.png'))
        x=(n%cols)*260+10;y=(n//cols)*360
        d.text((x,y+8),label,fill='#222222');sheet.paste(im,(x,y+30))
        parts.append('<figure><figcaption>'+html.escape(label)+'</figcaption><img src="example-'+name+'.png"></figure>')
    parts.append('</section>');sheet.save(out/('all-screens-'+str(idx)+'.png'))
parts.append('</html>');(out/'all-screens.html').write_text(''.join(parts),encoding='utf-8')
print('Exported',sum(len(items) for _,items in groups),'firmware examples and 3 overview images')
