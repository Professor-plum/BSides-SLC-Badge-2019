#ifndef StreetRoom_h
#define StreetRoom_h

#include "Room.h"
#include "FlashReader.h"
#include "LedWheel.h"
#include "NES.h"

#define PERSONW 37
#define PERSONH 76
#define PERSONOX 35
#define PERSONOY 195

bool peers[5];
uint8_t peer_id;

class StreetRoom: public Room {
  public:
  StreetRoom() : say(false) , sho(false){};
  ~StreetRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  private:
   roomID n;
   bool say, sho;
};

void StreetRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (wheel_get(C_SOCIAL)==true) FR.blt("street2.raw", tft, 0, 0, 240, 320);
  else FR.blt("street1.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

roomID StreetRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (peers[0] && peers[1] && peers[2] && peers[3] && peers[4] && (wheel_get(C_SOCIAL)==false)) {
    wheel_set(C_SOCIAL, true);
    FR.blt("street2.raw", tft, 0, 0, 240, 320);
  }
  
  if (peers[0]) {
    int mod = now % 800;
    if (mod<500) FR.blt("persona1.raw", tft, PERSONOX, PERSONOY, PERSONW, PERSONH);
    else if (mod<600) FR.blt("persona2.raw", tft, PERSONOX, PERSONOY, PERSONW, PERSONH);
    else if (mod<700) FR.blt("persona3.raw", tft, PERSONOX, PERSONOY, PERSONW, PERSONH);
    else FR.blt("persona2.raw", tft, PERSONOX, PERSONOY, PERSONW, PERSONH);
  }

  if (peers[1]) {
    int mod = now % 400;
    if (mod<200) FR.blt("personb1.raw", tft, PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else FR.blt("personb2.raw", tft, PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
  }

  if (peers[2]) {
    int mod = now % 1100;
    if (mod<500) FR.blt("personc1.raw", tft, 2*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else if (mod<700) FR.blt("personc2.raw", tft, 2*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else if (mod<900) FR.blt("personc3.raw", tft, 2*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else FR.blt("personc2.raw", tft, 2*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
  }

  if (peers[3]) {
    int mod = now % 1000;
    if (mod<500) FR.blt("persond1.raw", tft, 3*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else FR.blt("persond2.raw", tft, 3*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
  }

  if (peers[4]) {
    int mod = now % 5000;
    if (mod<2000) FR.blt("persone1.raw", tft, 4*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    else {
      mod = mod % 200;
      if (mod<100) FR.blt("persone2.raw", tft, 4*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
      else FR.blt("persone1.raw", tft, 4*PERSONW + PERSONOX, PERSONOY, PERSONW, PERSONH);
    }
  }

  if (say) {
    say = false;
    tft->fillRoundRect(93, 52, 98, 15, 3, ILI9341_WHITE);
    tft->drawRoundRect(93, 52, 98, 15, 3, ILI9341_BLACK);
    tft->fillTriangle(127, 65, 148, 66, 139, 92, ILI9341_WHITE);
    tft->drawLine(127, 66, 138, 92, ILI9341_BLACK);
    tft->drawLine(149, 66, 140, 92, ILI9341_BLACK);
    tft->setFont(&Pixel_NES5pt7b);
    tft->setTextColor(ILI9341_BLACK);
    tft->setCursor(98, 62);
    tft->print("Unauthorized");
  }

  if (sho) {
    FR.blt("sthint.raw", tft, 4, 105, 18,38);
    sho = false;
  }

  const char* unite = "U\0N\0I\0T\0E\0";
  tft->setFont(&Pixel_NES5pt7b);
  tft->setTextColor(ILI9341_WHITE);
  tft->setCursor(230, 12);
  tft->print(&unite[2*(peer_id-1)]);
  return n;
}

void StreetRoom::touchDown(int x, int y) {
  if ((x<16) && (y>230)) {
    n = R_NIGHTCLUB;
  }
  if ((x>4) && (x<22) && (y>110) && (y<130))
    sho = true;
  if ((x>70) && (x<200) && (y<140)) {
    if (wheel_get(C_SOCIAL)) 
      n = R_TOWER;
    else
    {
      say = true;
      wavPlay("halt2.wav");
    }
  }
}

#endif
