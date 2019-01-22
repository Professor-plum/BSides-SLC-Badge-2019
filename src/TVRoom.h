#ifndef TVRoom_h
#define TVRoom_h

#include "Room.h"

const int TVx = 40;
const int TVy = 160;

class TVRoom: public Room {
  public:
  TVRoom() {ears[0]=3;ears[1]=3;};
  ~TVRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void reset();
  
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  private:
    int ears[2];
    bool out;
};

#endif

