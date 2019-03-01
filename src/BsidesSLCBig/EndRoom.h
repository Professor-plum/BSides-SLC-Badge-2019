#ifndef EndRoom_h
#define EndRoom_h

#include "Room.h"
#include "FlashReader.h"
#include <Fonts/nes.h>

#define shades 32

const char* credits[] = {
"Your actions served as a     ",
"catalyst to unite the people ",
"and started a movement of ",
"independence. The people have ",
"taken responsibility and ",
"control from the overarching ",
"powers.",
"Things are not perfect," ,
"conflicts still arise but ",
"everyone is committed to ",
"finding solutions and no ",
"longer look to one authority ", 
"to solve their problems.",
"The city is at peace",
"for now...",
"",
"The End"
};

class EndRoom: public Room {
public:
  EndRoom() {};
  ~EndRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void touchDown(int x, int y); 
private:
  uint32_t last;
  uint32_t t, ii;
};

void EndRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  tft->fillScreen(ILI9341_WHITE);
  tft->setFont(&Pixel_NES5pt7b);
  ii=0;
  t=0;
}

roomID EndRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  int c = 255 - (ii % shades)*(256/shades); 
  int i= ii/shades;
  if (i<17) {
    int iy = 10 + i*12;
    tft->setFont(&Pixel_NES5pt7b);
    tft->setCursor(10, iy);
    tft->setTextColor(tft->color565(c,c,c));
    tft->print(credits[i]);
    tft->setFont();
    ++ii;
  }
  else if (ii==17*shades) {
    tft->setFont(&Pixel_NES5pt7b);
    for (int j=0; j<17; ++j) {
      tft->setCursor(10, 10 + j*12);
      tft->setTextColor(ILI9341_BLACK);
      tft->print(credits[j]);
    }
    tft->setFont();
    ++ii;
  }
  if (t) return R_SPLASH;
  return R_NOCHANGE;
}

void EndRoom::touchDown(int x, int y) {
  if (ii > 17*shades)
    t=1;
}

#endif

