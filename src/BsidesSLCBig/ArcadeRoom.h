#ifndef ArcadeRoom_h
#define ArcadeRoom_h

#include "Room.h"
#include "FlashReader.h"


class ArcadeRoom: public Room {
  public:
  ArcadeRoom() : n(R_NOCHANGE) {};
  ~ArcadeRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
  roomID n;
  int phase;
};

void ArcadeRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("arcade.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
  if (wheel_get(C_GAME))
    FR.blt("arcaded.raw", tft, 221, 119, 19, 55);
}

roomID ArcadeRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char o[12];
  int at = (now/800) % 4;
  if (at != phase) {
    snprintf(o, 12, "arcade%d.raw", at+1);
    FR.blt(o, tft, 65, 101, 138, 59);
    phase = at;
  }
  return n;
}

void ArcadeRoom::touchDown(int x, int y) {
  if ((x>50) && (x<120) && (y<32)) {
    n = R_DJ;
  }
  if ((x>64) && (x<88) && (y>115) && (y<152)) {
    n = R_GAME;
  }
  if (wheel_get(C_GAME) && (x>221) && (y>120)  && (y<200))
    n = R_PUZZLE;
}

#endif

