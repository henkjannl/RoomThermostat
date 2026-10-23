#pragma once
// Small host raster surface for testing the real firmware UI, not ESP32 hardware.
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>
#define PROGMEM
#define pgm_read_byte(p) (*(const uint8_t*)(p))
#define pgm_read_word(p) (*(const uint16_t*)(p))
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define constrain(x,a,b) (min(max(x,a),b))
class Adafruit_GFX {
public:
  int w,h;std::vector<uint16_t> pixels;
  Adafruit_GFX(int width=240,int height=320):w(width),h(height),pixels(w*h){}
  virtual ~Adafruit_GFX()=default;
  void drawPixel(int x,int y,uint16_t c){if(x>=0&&x<w&&y>=0&&y<h)pixels[y*w+x]=c;}
  void fillRect(int x,int y,int width,int height,uint16_t c){for(int j=0;j<height;++j)for(int i=0;i<width;++i)drawPixel(x+i,y+j,c);}
  void fillScreen(uint16_t c){std::fill(pixels.begin(),pixels.end(),c);}
  void drawFastHLine(int x,int y,int n,uint16_t c){fillRect(x,y,n,1,c);}
  void drawLine(int x,int y,int endX,int endY,uint16_t c){int dx=abs(endX-x),sx=x<endX?1:-1,dy=-abs(endY-y),sy=y<endY?1:-1,error=dx+dy;for(;;){drawPixel(x,y,c);if(x==endX&&y==endY)break;int e=2*error;if(e>=dy){error+=dy;x+=sx;}if(e<=dx){error+=dx;y+=sy;}}}
  void drawCircle(int x,int y,int r,uint16_t c){for(int i=0;i<180;++i){float t=i*6.2831853f/180;drawPixel(x+lroundf(r*cosf(t)),y+lroundf(r*sinf(t)),c);}}
  void drawRGBBitmap(int x,int y,const uint16_t*data,int width,int height){for(int j=0;j<height;++j)for(int i=0;i<width;++i)drawPixel(x+i,y+j,data[j*width+i]);}
  void setFont(void*){}void setTextSize(int){}void setTextWrap(bool){}void setTextColor(uint16_t,uint16_t){}void setCursor(int,int){}void print(const char*){}
};
class Adafruit_ILI9341:public Adafruit_GFX {};
class GFXcanvas16:public Adafruit_GFX {public:GFXcanvas16(int w,int h):Adafruit_GFX(w,h){}uint16_t*getBuffer(){return pixels.data();}};
