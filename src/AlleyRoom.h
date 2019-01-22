#ifndef AlleyRoom_h
#define AlleyRoom_h

#include "Room.h"
#include "FlashReader.h"

//const char* hash = "$1$63eae190$owyZl4VPBZGPLqyJwdD6/0";

const char* garbage[] = {
"Crash Dump:",
"A;.UiUaI.....;ayUT/\\ h.C.A;....p",
"Ao&1@eGp\"-;0C7iF....O`C.../etc/s",
"hadow.; p<Gx'E?`yyp...;A~;hA;0u;",
"p........a<iEU,A;@ U\"rAm,`d DuLA",
"Ao....&18eGpC;+C;b.....$1$63eae1",
"90$owyZl4VPBZGPLqyJwdD6/0....p..",
"......iEU A;.U)...'i1p^; a<Gx'..",
"..E?0..;agS;Gx'E?u;`.......Z;Gx'",
"}..EUuWcB?eOLaCL^......!&A6~!DpL"
};

enum ally_state {
  NONE,
  SPRAY,
  SHOW,
  SHOWING,
  REDRAW
};

class AlleyRoom: public Room {
  public:
  AlleyRoom(): astate(NONE), n(R_NOCHANGE) {};
  ~AlleyRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void touchDown(int x, int y);
  void touchMove(int x, int y);
  private:
    ally_state  astate;
    int corrds[2];
    roomID n;
};

void AlleyRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("alley.raw", tft, 0, 0, 240, 320);
  if (random(10) == 0) {
    FR.blt("Alleyart.raw", tft, 74, 116, 131, 42);
  }
  n = R_NOCHANGE;
}

roomID AlleyRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (astate == SHOW) {
    tft->fillRect(20,115,200,120, tft->color565(246,246,174));
    //FR.blt("trash.raw", tft, 5,215,235,320);
    tft->setTextColor(ILI9341_BLACK);
    for (int i=0; i<10; ++i) {
       tft->setCursor(25, 120 + i*10);
       tft->print(garbage[i]);
    }
    astate = SHOWING;
  }
  else if (astate == REDRAW) {
    refresh(tft, now);
    astate = NONE;
  }
  else if (astate == SPRAY) {
    tft->fillCircle(corrds[0], corrds[1], 1, ILI9341_PINK);
    astate = NONE;
  }
  return n;
}

void AlleyRoom::touchDown(int x, int y) {
  if (astate == SHOWING) {
    astate = REDRAW;
  }
  if ((x>216) && (y>256)) {
    n = R_MARKET;
  }
  if ((x>56) && (x<96) && (y>211) && (y<232)) {
    astate = SHOW;
  }
}

void AlleyRoom::touchMove(int x, int y) {
  if (y>256) return;
  if ((y<80) && (x<64)) return;
  if ((x<120) && (y>195)) return;
  astate = SPRAY;
  corrds[0]=x;
  corrds[1]=y;
}

#endif

