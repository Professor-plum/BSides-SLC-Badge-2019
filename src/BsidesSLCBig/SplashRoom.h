#ifndef SplashRoom_h
#define SplashRoom_h

#include "Room.h"
#include "FlashReader.h"
#include <Fonts/nes.h>

#define shades 32

void WriteFlash(Adafruit_ILI9341_STM *tft);

const char* intro[] = {
"Intolerance and feuds were",
"pervasive throughout the city.", 
"To prevent these riots, a new",
"program was initiated which",
"divided the city into sectors.", 
"Robots now enforce these",
"boundaries to keep the peace.", 
"All activity is monitored and",
"only state approved media",
"remains. Self expression,",
"independent broadcasting, and", 
"travel are all security risks",
"and outlawed. Those who resist",
"are punished...",
"There are those who are", 
"spreading the message to",
"embrace individuality and band",
"together to topple the",
"repressive system.",
"",
};

class SplashRoom: public Room {
public:
  SplashRoom() {};
  ~SplashRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void touchDown(int x, int y);
  void touchUp(int x, int y); 
private:
  uint32_t last;
  uint32_t t, ii;
  bool config;
};

void SplashRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  tft->fillScreen(ILI9341_WHITE);
  FR.blt("logo.raw", tft, 0,0,240,240);
  tft->setCursor(100, 280);
  tft->setTextColor(ILI9341_BLACK);
  tft->setFont(&Pixel_NES5pt7b);
  tft->print("2019");
  tft->setFont();
  t=0;
  ii=0;
  config=false;
}

roomID SplashRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (t == 0) {
    if ((now % 1000) < 500) {
      tft->setFont(&Pixel_NES5pt7b);
      tft->setCursor(64, 256);
      tft->setTextColor(ILI9341_BLACK);
      tft->print("Touch to start");
      tft->setFont();
    }
    else {
      tft->fillRect(32, 244, 192, 16, ILI9341_WHITE);
    }
  }
  else {
      if (ii==0)
        tft->fillScreen(ILI9341_WHITE);
      
      int c = 255 - (ii % shades)*(256/shades); 
      int i= ii/shades;
      if (i<20) {
        int iy = 10 + i*12;
        tft->setFont(&Pixel_NES5pt7b);
        tft->setCursor(10, iy);
        tft->setTextColor(tft->color565(c,c,c));
        tft->print(intro[i]);
        tft->setFont();
        ++ii;
      }
      else if (ii==20*shades) {
        tft->setFont(&Pixel_NES5pt7b);
        for (int j=0; j<20; ++j) {
          tft->setCursor(10, 10 + j*12);
          tft->setTextColor(ILI9341_BLACK);
          tft->print(intro[j]);
        }
        tft->setFont();
        ++ii;
      }
  }
  last = now;
  if (config) return R_CONFIGURE;
  if (t>1) return R_TV;
  return R_NOCHANGE;
}

void SplashRoom::touchDown(int x, int y) {
  if (t==1 && (ii < 20*shades))
    ii = 20*shades;
  else {
    t++;
    if (t==1)
      ii = 0;
  }
}

void SplashRoom::touchUp(int x, int y) {
  if (t==1 && (ii > 5*shades) && (ii < 20*shades)) config = true;
}

#endif

