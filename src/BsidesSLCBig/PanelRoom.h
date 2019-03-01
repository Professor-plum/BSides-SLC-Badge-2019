#ifndef PanelRoom_h
#define PanelRoom_h

#include "Room.h"
#include "FlashReader.h"
#include "NES.h"

#define KNOB1_LOC  26,220,25,25
#define KNOB2_LOC  190,220,25,25
#define SWITCH1_LOC 7,274,16,41
#define SWITCH2_LOC 31,274,16,41
#define SWITCH3_LOC 55,274,16,41
#define SLIDER1_LOC 85,244,76,12
#define SLIDER2_LOC 85,264,76,12
#define SLIDER3_LOC 85,284,76,12
#define SLIDER4_LOC 85,304,76,12
#define BUTTON1_LOC 77,208,24,25
#define BUTTON2_LOC 110,208,24,25
#define BUTTON3_LOC 143,208,24,25

#define SCREEN1_LOC 152,41,30,21
#define SCREEN2_LOC 191,41,30,21
#define SCREEN3_LOC 152,75,30,21
#define SCREEN4_LOC 191,75,30,21


#define ON_AIR 90,46,25,7
#define GO_LOC 184,274,37,39

class PanelRoom: public Room {
  public:
  PanelRoom() : knobs{0,0}, switches{false, false, false}, sliders {0,0,0,0}, buttons{false, false, false}, _go(false), li(5), played(false), solved(false){};
  ~PanelRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchUp(int x, int y);
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  
  private:
    bool inBox(int px, int py, int x, int y, int w, int h);
    int knobs[2];
    bool switches[3];
    int sliders[4];
    bool buttons[3];
    bool _go;
    bool redraw, played, solved;
    roomID n;
    char li;
};

void PanelRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("control.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  redraw=true;
  update(tft, now);
}

roomID PanelRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char l[15];

  if (!played) FR.blt("offair.raw", tft, ON_AIR);

  if (redraw) {
    sprintf(l, "knob%d.raw", knobs[0]+1);
    FR.blt(l, tft, KNOB1_LOC);
  
    sprintf(l, "knob%d.raw", knobs[1]+1);
    FR.blt(l, tft, KNOB2_LOC);
  
    if (switches[0]) FR.blt("switchup.raw", tft, SWITCH1_LOC);
    else FR.blt("switchdn.raw", tft, SWITCH1_LOC);
    if (switches[1]) FR.blt("switchup.raw", tft, SWITCH2_LOC);
    else FR.blt("switchdn.raw", tft, SWITCH2_LOC);
    if (switches[2]) FR.blt("switchup.raw", tft, SWITCH3_LOC);
    else FR.blt("switchdn.raw", tft, SWITCH3_LOC);
    
    if (sliders[0] == 0) FR.blt("sliderdn.raw", tft, SLIDER1_LOC);
    else if (sliders[0] == 1) FR.blt("slidermd.raw", tft, SLIDER1_LOC);
    else FR.blt("sliderup.raw", tft, SLIDER1_LOC);
    
    if (sliders[1] == 0) FR.blt("sliderdn.raw", tft, SLIDER2_LOC);
    else if (sliders[1] == 1) FR.blt("slidermd.raw", tft, SLIDER2_LOC);
    else FR.blt("sliderup.raw", tft, SLIDER2_LOC);
    
    if (sliders[2] == 0) FR.blt("sliderdn.raw", tft, SLIDER3_LOC);
    else if (sliders[2] == 1) FR.blt("slidermd.raw", tft, SLIDER3_LOC);
    else FR.blt("sliderup.raw", tft, SLIDER3_LOC);
  
    if (sliders[3] == 0) FR.blt("sliderdn.raw", tft, SLIDER4_LOC);
    else if (sliders[3] == 1) FR.blt("slidermd.raw", tft, SLIDER4_LOC);
    else FR.blt("sliderup.raw", tft, SLIDER4_LOC);
  
    if (buttons[0]) FR.blt("buttondn.raw", tft, BUTTON1_LOC);
    else FR.blt("buttonup.raw", tft, BUTTON1_LOC);
    if (buttons[1]) FR.blt("buttondn.raw", tft, BUTTON2_LOC);
    else FR.blt("buttonup.raw", tft, BUTTON2_LOC);
    if (buttons[2]) FR.blt("buttondn.raw", tft, BUTTON3_LOC);
    else FR.blt("buttonup.raw", tft, BUTTON3_LOC);
  
    if (_go) {
      FR.blt("godn.raw", tft, GO_LOC);
      if (wheel_get(C_CRYPTO)) {
        FR.blt("onair.raw", tft, ON_AIR);
        tft->fillRoundRect(5,0,230,15,5, ILI9341_WHITE);
        tft->drawRoundRect(5,0,230,15,5, ILI9341_BLACK);
        tft->setCursor(10, 10);
        tft->setFont(&Pixel_NES5pt7b);
        tft->setTextColor(ILI9341_BLACK);
        tft->print("I am a hacker, enter my world...");
      }
    }
    else FR.blt("goup.raw", tft, GO_LOC);
    redraw = false;
  }

  if (played && !wavIsPlaying())
        return R_END;

  int i = (now/100)%4;
  if (i != li) {
    li = i;
    if (solved) {
      snprintf(l, 15, "maxhead%d.raw", i);
      FR.blt(l, tft, 191, 41, 30, 21);
    }
    if (solved && wheel_get(C_CODE)) {
        snprintf(l, 15, "maxhead%d.raw", i);
        FR.blt(l, tft, 152, 41, 30, 21);
    }
    else {
      snprintf(l, 15, "screena%d.raw", i/2);
      FR.blt(l, tft, 152, 41, 30, 21);
    }
    if (solved && wheel_get(C_PUZZLE)) {
        snprintf(l, 15, "maxhead%d.raw", i);
        FR.blt(l, tft, 152, 75, 30, 21);
    }
    else {
      snprintf(l, 15, "screenc%d.raw", i);
      FR.blt(l, tft, 152, 75, 30, 21);
    }
    if (solved && wheel_get(C_SOLDER)) {
        snprintf(l, 15, "maxhead%d.raw", i);
        FR.blt(l, tft, 191, 75, 30, 21);
    }
    else {
      snprintf(l, 15, "screend%d.raw", i);
      FR.blt(l, tft, 191, 75, 30, 21);
    }
  }
  return n;
}

void PanelRoom::touchDown(int x, int y) {
  redraw = true;
  if (inBox(x,y, GO_LOC)) {
    _go = true;
    if ((knobs[0] == 3) &&
       (knobs[1] == 5) &&
       (switches[0] == false) && 
       (switches[1] == true) && 
       (switches[2] == true) && 
       (sliders[0] == 1) && 
       (sliders[1] == 0) &&
       (sliders[2] == 2) && 
       (sliders[3] == 1) && 
       (buttons[0] == true) && 
       (buttons[1] == false) && 
       (buttons[2] == true)) {
        wheel_set(C_CRYPTO, true);
        if (wheel_complete()) {
          wavPlay("endingb.wav");
          played = true;
        }
        else {
          wavPlay("endingb.wav");
        }
        solved=true;
       }
  }
  else 
    _go = false;

  if ((x>4) && (x<38) && (y>64) && (y<128)) {
    n = R_TOWER;
  }
  if (inBox(x,y, KNOB1_LOC)) {
    knobs[0] = (knobs[0]+1)%7;
  }
  else if (inBox(x,y, KNOB2_LOC)) {
    knobs[1] = (knobs[1]+1)%7;
  }
  else if (inBox(x,y, SWITCH1_LOC))
    switches[0] = !switches[0];
  else if (inBox(x,y, SWITCH2_LOC)) 
    switches[1] = !switches[1];
  else if (inBox(x,y, SWITCH3_LOC)) 
    switches[2] = !switches[2];
  else if (inBox(x,y, SLIDER1_LOC)) 
    sliders[0] = (sliders[0] + 1)%3;
  else if (inBox(x,y, SLIDER2_LOC)) 
    sliders[1] = (sliders[1] + 1)%3;
  else if (inBox(x,y, SLIDER3_LOC)) 
    sliders[2] = (sliders[2] + 1)%3;
   else if (inBox(x,y, SLIDER4_LOC)) 
    sliders[3] = (sliders[3] + 1)%3;
  else if (inBox(x,y, BUTTON1_LOC)) 
    buttons[0] = !buttons[0];
  else if (inBox(x,y, BUTTON2_LOC)) 
    buttons[1] = !buttons[1];
  else if (inBox(x,y, BUTTON3_LOC)) 
    buttons[2] = !buttons[2];
}

void PanelRoom::touchMove(int x, int y) {
  
}

void PanelRoom::touchUp(int x, int y) {
  _go = false;
  redraw = true;
}

bool PanelRoom::inBox(int px, int py, int x, int y, int w, int h) {
  return (px >= x) && (px < (x+w)) && (py >= y)  && (py < (y+h));
}

#endif
