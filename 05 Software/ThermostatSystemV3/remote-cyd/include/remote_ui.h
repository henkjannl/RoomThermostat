#pragma once
#include <Adafruit_ILI9341.h>
#include <font_assets.h>
#include <display_assets.h>

// Draw shared original VLW fonts directly from flash, without font heap tables
// or a filesystem upload. The menu renderer optionally uses a framebuffer.
namespace RemoteUi {
constexpr uint16_t White=0xFFFF, Ink=0x2104, Grey=0x6B6D;
constexpr uint16_t Line=0xE71C, Panel=White, Green=0x2C6B, Amber=0xBB00;
inline const FlashImage* image(const char*name){for(const auto &a:flashImages)if(!strcmp(a.name,name))return &a;return nullptr;}
inline uint16_t blend(uint16_t,uint16_t,uint8_t);
inline void crop(Adafruit_GFX &lcd,const char*name,int sx,int sy,int sw,int sh,int x,int y,int w,int h,uint8_t alpha=255){
  const auto*a=image(name);if(!a)return;
  uint16_t row[64];if(w>64)return;
  for(int j=0;j<h;++j){for(int i=0;i<w;++i){uint16_t pixel=pgm_read_word(a->pixels+(sy+j*sh/h)*a->width+sx+i*sw/w);row[i]=alpha==255?pixel:blend(pixel,White,alpha);}lcd.drawRGBBitmap(x,y+j,row,w,1);}
}
inline void icon(Adafruit_GFX &lcd,const char*name,int x,int y,int size=32,uint8_t alpha=255){const auto*a=image(name);if(a)crop(lcd,name,0,0,a->width,a->height,x,y,size,size,alpha);}
inline int32_t word(const uint8_t *p) {
  uint32_t v=0;
  for (int i=0;i<4;++i) v=(v<<8)|pgm_read_byte(p+i);
  return static_cast<int32_t>(v);
}
struct Glyph {int h,w,advance,dy,dx; const uint8_t *pixels;};
inline Glyph glyph(const uint8_t *font,uint8_t code) {
  if (code==0xB0) return {0,0,7,0,0,nullptr}; // Original VLW has no degree glyph.
  int count=word(font);
  const uint8_t *pixels=font+24+count*28;
  for (int i=0;i<count;++i) {
    const uint8_t *m=font+24+i*28;
    int h=word(m+4),w=word(m+8);
    if (word(m)==code) return {h,w,word(m+12),word(m+16),word(m+20),pixels};
    pixels+=h*w;
  }
  return {0,0,word(font+8)/3,0,0,nullptr}; // Includes spaces.
}
inline int width(const uint8_t *font,const char *text) {
  int result=0;
  while (*text) result+=glyph(font,uint8_t(*text++)).advance;
  return result;
}
inline uint16_t blend(uint16_t fg,uint16_t bg,uint8_t alpha) {
  uint32_t inv=255-alpha;
  unsigned r=(((fg>>11)&31)*alpha+((bg>>11)&31)*inv+127)/255;
  unsigned g=(((fg>>5)&63)*alpha+((bg>>5)&63)*inv+127)/255;
  unsigned b=((fg&31)*alpha+(bg&31)*inv+127)/255;
  return (r<<11)|(g<<5)|b;
}
inline void text(Adafruit_GFX &lcd,const uint8_t *font,const char *value,
                 int x,int top,uint16_t fg=Ink,uint16_t bg=White) {
  uint16_t row[128];
  int baseline=top+word(font+16);
  while (*value) {
    uint8_t code=uint8_t(*value++);
    Glyph g=glyph(font,code);
    if (code==0xB0) lcd.drawCircle(x+3,top+3,2,fg);
    for (int y=0;y<g.h;++y) {
      for (int start=0;start<g.w;start+=128) {
        int n=min(128,g.w-start);
        for (int k=0;k<n;++k)
          row[k]=blend(fg,bg,pgm_read_byte(g.pixels+y*g.w+start+k));
        lcd.drawRGBBitmap(x+g.dx+start,baseline-g.dy+y,row,n,1);
      }
    }
    x+=g.advance;
  }
}
inline void centered(Adafruit_GFX &lcd,const uint8_t *font,const char *value,
                     int top,uint16_t fg=Ink) {
  text(lcd,font,value,(240-width(font,value))/2,top,fg);
}
inline void footer(Adafruit_GFX &lcd,const char *version) {
  lcd.setFont(nullptr);lcd.setTextSize(1);lcd.setTextWrap(false);
  lcd.setTextColor(Grey,White);
  lcd.setCursor((240-6*strlen(version))/2,303);lcd.print(version);
}
inline void wake(Adafruit_GFX &lcd,const char *version) {
  lcd.fillScreen(White);
  centered(lcd,font_Noway_Medium24,"Even verbinden",124);
  centered(lcd,font_Noway_Light18,"Temperaturen ophalen",158,Grey);
  footer(lcd,version);
}
}
#include "remote_menu.h"
