#ifndef TowerRoom_h
#define TowerRoom_h

#include "Room.h"
#include "FlashReader.h"
#include <LedWheel.h>

#define NOTPIN PB15

class TowerRoom: public Room {
  public:
  TowerRoom(): sho(false) {};
  ~TowerRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
    roomID n;
    bool sho;
};

void TowerRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("tower.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

bool testXor() {
  bool ret, ret2;
  bool val = digitalRead(PB12);
  digitalWrite(PB12, HIGH);
  ret =  (digitalRead(NOTPIN) == LOW) ;
  digitalWrite(PB12, LOW);
  ret2 = (digitalRead(NOTPIN) == HIGH);
  digitalWrite(PB12, val);
  return ret && ret2;
}

roomID TowerRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char fname[15];
  if (testXor()) {
    wheel_set(C_SOLDER, true);
    FR.blt("towerp.raw", tft, 88, 199, 7, 25);
  }
  else {
    //draw spark
    int i = (now/100)%6;
    snprintf(fname, 15, "spark%d.raw", i);
    FR.blt(fname, tft, 86, 208, 9, 9);
  }

  //draw bird
  int i = (now%6000);
  if (i<2900)
    FR.blt("bird1.raw", tft, 21, 135, 7, 7);
  else if (i<3000)
    FR.blt("bird2.raw", tft, 21, 135, 7, 7);
  else if (i<5900)
    FR.blt("bird3.raw", tft, 21, 135, 7, 7);
  else 
    FR.blt("bird2.raw", tft, 21, 135, 7, 7);

  if (sho) {
    FR.blt("towhint.raw", tft, 207, 276, 33, 44);
    sho = false;
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
  if ((x>21) && (x<28) && (y>135) && (y<142)) {
    wavPlay("chirp.wav");
  }

  if ((x>212) && (y>283))
    sho =true;
}

#endif

