#ifndef KeypadRoom_h
#define KeypadRoom_h

#include "Room.h"
#include "FlashReader.h"

const int k_key[] = { 6, 3, 1, 7, 0, 2}; //742813

class KeypadRoom: public Room {
public:
  KeypadRoom(): _keydown(-1), _lastKey(-1), solved(false) , idx(0){};
  ~KeypadRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  void touchUp(int x, int y);
  void reset() {idx=0; solved=false;};
  private:
    int _keydown, _lastKey;
    int number[6], idx;
    bool solved;
    roomID n;
};


void KeypadRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char filename[14];
  tft->fillScreen(ILI9341_LIGHTGREY);
  tft->fillRoundRect(2,2,236,316, 4, ILI9341_BLACK);
  for (int i=0; i<12; ++i) {
    sprintf(filename, "keypad%d.raw", i+1);
    //tft->fillRect(8 + 80*(i%3), 8 + 80*(i/3), 64, 64, ILI9341_LIGHTGREY);
    FR.blt(filename, tft, 24 + 64*(i%3), 24 + 64*(i/3), 64, 64);
  }
  tft->fillCircle(180, 300, 12, ILI9341_MAROON);
  tft->fillCircle(220, 300, 12, ILI9341_DARKGREEN); 
  n = R_NOCHANGE;
}

roomID KeypadRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (_lastKey != _keydown) {
    if (_keydown != -1) {
      tft->fillCircle(180, 300, 12, ILI9341_MAROON);
      tft->fillCircle(220, 300, 12, ILI9341_GREEN);
      wavPlay("beep.wav");
      if (idx < 6) {
        number[idx++] = _keydown;
        if (idx>=6) {
          if (memcmp(number, k_key, 6)==0) {
            wheel_set(C_KEYPAD, true);
            solved = true;
          }
          else
          {
            tft->fillCircle(180, 300, 12, ILI9341_RED);
            tft->fillCircle(220, 300, 12, ILI9341_DARKGREEN);
            wavPlay("wrong.wav");
            idx=0;
          }
        }
      }
      _lastKey = _keydown;
    }
    else { //no key
      tft->fillCircle(220, 300, 12, ILI9341_DARKGREEN);
    }
  }
  if (solved && !wavIsPlaying()) return R_PANEL;
  return n;
}

void KeypadRoom::touchDown(int x, int y) {
  touchMove(x, y);
  if (_keydown == 9) {
    reset();
    n = R_TOWER;
  }
}

void KeypadRoom::touchMove(int x, int y) {
  if ((x>24) && (x<216) && (y>32) && (y<288)) {
    x-=24;
    y-=32;
    _keydown = (x/64) + 3*(y/64);
  }
  else
    _keydown = -1;
}

void KeypadRoom::touchUp(int x, int y) {
    _keydown=-1;
}
#endif

