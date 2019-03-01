#ifndef NightClub_h
#define NightClub_h

#include "Room.h"
#include "FlashReader.h"

#define LIGHT     PA0

class NightClub: public Room {
public:
  NightClub(): night(false) ,vals(0) {};
  ~NightClub() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
    bool night;
    int vals, phase;
    roomID n;
};


void NightClub::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (night)
    FR.blt("nclub2.raw", tft, 0, 0, 240, 320);
  else
    FR.blt("nclub1.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
}

roomID NightClub::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (((analogRead(LIGHT) + analogRead(LIGHT) + analogRead(LIGHT))/3) < 15) {
    vals++;
  }
  else
    vals=0;
  bool dark = vals > 10;
  if (night ^ dark) {
    night = dark;
    refresh(tft, now);
    if (night)
      wheel_set(C_SENSOR, true);
  }
  if (dark) {
    uint32_t tt = now % 3000;
    if ((phase != 0) && (tt<500) && ((tt%200) < 100)) {
      //tft->fillRect(33,45,26,31, ILI9341_BLACK);
      FR.blt("nclubn.raw", tft, 33, 45, 26, 31);
      phase=0;
    }
    else if (phase !=1) {
      FR.blt("nclubl.raw", tft, 33, 45, 26, 31);
      phase=1;
    }
  }
  return n;
}


void NightClub::touchDown(int x, int y) {
  if ((x<40) && (y>220))
    n = R_MARKET;

  if (night && (x>16) && (x<40) && (y>136) && (y<220)) {
    n = R_DJ;
  }

  if ((x>200) && (y>120) && (y<200))
    n = R_STREET;

  /*if ((x>200) && (y>280)) {
    wavPlay("error.wav");
  }*/
}

#endif

