#ifndef TowerRoom_h
#define TowerRoom_h

#include "Room.h"
#include "FlashReader.h"
#include <LedWheel.h>

#define NOTPIN PB15

class TowerRoom: public Room {
  public:
  TowerRoom() {};
  ~TowerRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
    roomID n;
};

void TowerRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("tower.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

bool testXor() {
  digitalWrite(PB12, HIGH);
  if (digitalRead(NOTPIN) == HIGH)
    return false;
  digitalWrite(PB12, LOW);
  return digitalRead(NOTPIN) == HIGH;
}

roomID TowerRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (testXor) {
    wheel_set(C_SOLDER, true);
    //draw fixed overlay
  }
  else {
    //draw spark
  }
  return n;
}

void TowerRoom::touchDown(int x, int y) {
  if ((y>176) && (y<208) && (x>200)) {
    n = R_STREET;
  }
  if ((x<24) && (y>256) && (y<288)) {
    n = R_KEYPAD;
  }
}

#endif

