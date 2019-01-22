#ifndef MarketRoom_h
#define MarketRoom_h

#include "Room.h"
#include "FlashReader.h"

#define COIN 197, 124, 5, 20

class MarketRoom: public Room {
  public:
  MarketRoom() {};
  ~MarketRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
    roomID n;
};

void MarketRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("market.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

roomID MarketRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  now = now % 1200;
  if (now < 500) 
    FR.blt("mcoin1.bmp", tft, COIN);
  else if (now < 600)
    FR.blt("mcoin2.bmp", tft, COIN);
  else if (now < 700)
    FR.blt("mcoin3.bmp", tft, COIN);
  else if (now < 800)
    FR.blt("mcoin4.bmp", tft, COIN);
  else if (now < 900)
    FR.blt("mcoin5.bmp", tft, COIN);
  else if (now < 1000)
    FR.blt("mcoin4.bmp", tft, COIN);
  else if (now < 1100)
    FR.blt("mcoin3.bmp", tft, COIN);
  else 
    FR.blt("mcoin2.bmp", tft, COIN);
  return n;
}

void MarketRoom::touchDown(int x, int y) {
  if ((x<24) && (y>256)) {
    n = R_ALLEY;
  }
  if ((x>216) && (y>152) && (y<232)) {
    n = R_NIGHTCLUB;
  }
  if ((x>120) && (x<168) && (y>88) && (y<200)) {
    n = R_TV;
  }
  if ((x>40) && (x<88) && (y>119) && (y<192)) {
    n = R_DIAL;
  }
}

#endif

