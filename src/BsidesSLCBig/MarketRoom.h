#ifndef MarketRoom_h
#define MarketRoom_h

#include "Room.h"
#include "FlashReader.h"
#include "NES.h"

#define COIN 197, 124, 5, 20

class MarketRoom: public Room {
  public:
  MarketRoom(): say(false) {};
  ~MarketRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
    roomID n;
    bool say;
};

void MarketRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("market.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

roomID MarketRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  now = now % 1200;
  if (now < 500) 
    FR.blt("mcoin1.raw", tft, COIN);
  else if (now < 600)
    FR.blt("mcoin2.raw", tft, COIN);
  else if (now < 700)
    FR.blt("mcoin3.raw", tft, COIN);
  else if (now < 800)
    FR.blt("mcoin4.raw", tft, COIN);
  else if (now < 900)
    FR.blt("mcoin5.raw", tft, COIN);
  else if (now < 1000)
    FR.blt("mcoin4.raw", tft, COIN);
  else if (now < 1100)
    FR.blt("mcoin3.raw", tft, COIN);
  else 
    FR.blt("mcoin2.raw", tft, COIN);

  if (say) {
    tft->fillRoundRect(35, 56, 188, 35, 3, ILI9341_WHITE);
    tft->drawRoundRect(35, 56, 188, 35, 3, ILI9341_BLACK);
    tft->fillTriangle(137, 90, 182, 115, 173, 90, ILI9341_WHITE);
    tft->drawLine(137, 90, 182, 115, ILI9341_BLACK);
    tft->drawLine(173, 90, 182, 115, ILI9341_BLACK);
    tft->setFont(&Pixel_NES5pt7b);
    tft->setTextColor(ILI9341_BLACK);
    tft->setCursor(43, 66);
    tft->print(" Here, take a coin and");
    tft->setCursor(43, 76);
    tft->print("go call a large number");
    tft->setCursor(43, 86);
    tft->print("    or something.");
    say = false;
    wavPlay("plum2.wav");
  }
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
  if ((x>180) && (x<202) && (y>106) && (y<182)) {
    say = true;
  }
}

#endif
