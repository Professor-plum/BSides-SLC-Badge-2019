#ifndef DJRoom_h
#define DJRoom_h

#include "Room.h"
#include "FlashReader.h"

const int notes[] = {1, 1, 5, 5, 3, 3, 2, 4, 2, 5};


class DJRoom: public Room {
  public:
  DJRoom() : note(0), img(0), n(R_NOCHANGE), sho(false) {};
  ~DJRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void  touchDown(int x, int y);
  void  touchUp(int x, int y);
  private:
    int note, img;
    roomID n;
    bool sho;
};

void DJRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("dj1.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

roomID DJRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (wheel_get(C_DJ)) {
    int at = (now % 1000) > 500;
    if (at != img) {
      img = at;
      if (img == 0) {
        FR.blt("dj2.raw", tft, 0, 45, 240, 275);
      }
      else  {
        FR.blt("dj3.raw", tft, 0, 45, 240, 275);
      }
    }
  }
  if (sho) {
    FR.blt("djm.raw", tft, 59, 18, 17, 27);
    sho=false;
  }
  return n;
}

void DJRoom::touchDown(int x, int y) {
  int key = -1;
  if ((x>42) && (x<79) && (y>272) && (y<309)) {
    key =1;
    wavPlay("note1.wav");
  }
  else if ((x>74) && (x<111) && (y>237) && (y<275)) {
    key =2;
    wavPlay("note2.wav");
  }
  else if ((x>102) && (x<130) && (y>272) && (y<301)) {
    key =3;
    wavPlay("note3.wav");
  }
  else if ((x>122) && (x<158) && (y>239) && (y<275)) {
    key =4;
    wavPlay("note4.wav");
  }
  else if ((x>156) && (x<193) && (y>272)  && (y<309)) {
    key =5;
    wavPlay("note5.wav");
  }
  
  if (key != -1) {
    if (key == notes[note]) note++;
    else if (key == notes[0]) note=1;
    else note = 0;
  }

  if (note == 10) {
    note = 0;
    wheel_set(C_DJ, true);
    wavPlay("tune2.wav");
  }

  if ((x>59) && (x<74) && (y>18) && (y<45)) {
    sho=true;
  }
  
}

void DJRoom::touchUp(int x, int y) {
  if ((x>222) && (y>100) && (y<180)) {
    n = R_NIGHTCLUB;
  }
  if (wheel_get(C_DJ) && (x>25) && (x<37) && (y>45) && (y<110)) {
    n = R_ARCADE;
  }
}

#endif

