#ifndef PanelRoom_h
#define PanelRoom_h

#include "Room.h"
#include "FlashReader.h"

#define KNOB_LOC  26,220,25,25
#define SWITCH1_LOC 7,274,16,41
#define SWITCH2_LOC 31,274,16,41
#define SWITCH3_LOC 55,274,16,41
#define SLIDER1_LOC 85,264,76,12
#define SLIDER2_LOC 85,284,76,12
#define SLIDER3_LOC 85,304,76,12
#define BUTTON1_LOC 174,252,24,25
#define BUTTON2_LOC 207,252,24,25
#define BUTTON3_LOC 174,291,24,25
#define BUTTON4_LOC 207,291,24,25

#define TIMEA_LOC 175,210,24,22
#define TIMEB_LOC 204,210,24,22

#define ON_AIR 101,15,25,7
#define GO_LOC 100,209,37,39

class PanelRoom: public Room {
  public:
  PanelRoom() : knob(0), switches{false, false, false}, sliders {0,0}, buttons{false, false, false, false}, _go(false){};
  ~PanelRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchUp(int x, int y);
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  
  private:
    bool inBox(int px, int py, int x, int y, int w, int h);
    int knob;
    bool switches[3];
    int sliders[3];
    bool buttons[4];
    bool _go;
    roomID n;
};

void PanelRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("control.raw", tft, 0, 0, 240, 320);
  n = R_NOCHANGE;
  update(tft, now);
}

roomID PanelRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  char l[10];
  sprintf(l, "knob%d.raw", knob+1);
  FR.blt(l, tft, KNOB_LOC);

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
  else FR.blt("sliderup.raw", tft, SLIDER2_LOC);

  if (buttons[0]) FR.blt("buttondn.raw", tft, BUTTON1_LOC);
  else FR.blt("buttonup.raw", tft, BUTTON1_LOC);
  if (buttons[1]) FR.blt("buttondn.raw", tft, BUTTON2_LOC);
  else FR.blt("buttonup.raw", tft, BUTTON2_LOC);
  if (buttons[2]) FR.blt("buttondn.raw", tft, BUTTON3_LOC);
  else FR.blt("buttonup.raw", tft, BUTTON3_LOC);
  if (buttons[3]) FR.blt("buttondn.raw", tft, BUTTON4_LOC);
  else FR.blt("buttonup.raw", tft, BUTTON4_LOC);

  if (_go) FR.blt("godn.raw", tft, GO_LOC);
  else FR.blt("goup.raw", tft, GO_LOC);

  if ((now % 1000) <500) {
    FR.blt("time0.raw", tft, TIMEA_LOC);
    FR.blt("time0.raw", tft, TIMEB_LOC);
  }
  else {
    FR.blt("timea.raw", tft, TIMEA_LOC);
    FR.blt("timeb.raw", tft, TIMEB_LOC);
  }
  return n;
}

void PanelRoom::touchDown(int x, int y) {
  if (inBox(x,y, GO_LOC)) {
    _go = true;
    if ((knob == 4) &&
       (switches[0] == false) && 
       (switches[1] == true) && 
       (switches[2] == true) && 
       (sliders[0] == 0) && 
       (sliders[1] == 1) &&
       (sliders[2] == 1) && 
       (buttons[0] == false) && 
       (buttons[1] == true) && 
       (buttons[2] == false) && 
       (buttons[3] == true)) {
        wheel_set(C_CRYPTO, true);
       }
       Serial.println("Message sent!");
       //playsound("message received")
  }
  else
    _go = false;

  if ((x>14) && (x<50) && (y>33) && (y<97)) {
    n = R_TOWER;
  }
  if (inBox(x,y, KNOB_LOC)) {
    knob = (knob+1)%7;
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
  else if (inBox(x,y, BUTTON1_LOC)) 
    buttons[0] = !buttons[0];
  else if (inBox(x,y, BUTTON2_LOC)) 
    buttons[1] = !buttons[1];
  else if (inBox(x,y, BUTTON3_LOC)) 
    buttons[2] = !buttons[2];
  else if (inBox(x,y, BUTTON4_LOC))
    buttons[3] = !buttons[3];
}

void PanelRoom::touchMove(int x, int y) {
  
}

void PanelRoom::touchUp(int x, int y) {
  _go = false;

}

bool PanelRoom::inBox(int px, int py, int x, int y, int w, int h) {
  return (px >= x) && (px < (x+w)) && (py >= y)  && (py < (y+h));
}

#endif

