#ifndef DialRoom_h
#define DialRoom_h

#include "Room.h"
#include "FlashReader.h"

const char key[] = { 3, 5, 5, 3, 5, 4, 6, 4, 2, 8}; 

class DialRoom: public Room {
public:
  DialRoom(): _keydown(-1), _lastKey(-1), solved(false), n(R_NOCHANGE), idx(0) {reset();};
  ~DialRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  void touchUp(int x, int y);
  void reset() {idx=0; _lastKey=-1; solved=false;};
  private:
    int _keydown, _lastKey;
    char number[10], idx;
    bool solved;
    roomID n;
};


void DialRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char filename[14];
  tft->fillScreen(tft->color565(94, 94, 94));
  for (int i=0; i<12; ++i) {
    sprintf(filename, "phon%dup.raw", i+1);
    FR.blt(filename, tft, 8+80*(i%3), 8+80*(i/3), 64, 64);
  }
  n = R_NOCHANGE;
}

roomID DialRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (_lastKey != _keydown) {
    char filename[14];
    if (_lastKey != -1) {
      sprintf(filename, "phon%dup.raw", _lastKey+1);
      FR.blt(filename, tft, 8+80*(_lastKey%3), 8+80*(_lastKey/3), 64, 64);
      if (idx < 10) {
        number[idx++] = _lastKey;
        if (idx==10) {
          if ( memcmp(number, key, 10)==0) {
            wheel_set(C_PHONE, true);
            wavPlay("modem.wav");
            solved = true;
          }
          else
          {
            wavPlay("wrongno2.wav");
          }
        }
      }
    }
    if (_keydown != -1) {
      sprintf(filename, "phon%ddn.raw", _keydown+1);
      FR.blt(filename, tft, 8+80*(_keydown%3), 8+80*(_keydown/3), 64, 64);
      sprintf(filename, "dtmf-%d.wav", _keydown+1);
      wavPlay(filename);
      
    }
    _lastKey = _keydown;
  }
  if (solved && !wavIsPlaying())
    return R_MAINFRAME;
  return n;
}

void DialRoom::touchDown(int x, int y) {
  _keydown = (x/80) + 3*(y/80);
  if (_keydown == 9) {
    reset();
    n = R_MARKET;
  }
}

void DialRoom::touchMove(int x, int y) {
}

void DialRoom::touchUp(int x, int y) {
  _keydown=-1;
}
#endif
