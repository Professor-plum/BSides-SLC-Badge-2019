#ifndef MainFrameRoom_h
#define MainFrameRoom_h

#include "Room.h"
#include "FlashReader.h"

enum keyk {
  NONE_KEY,
  A_KEY,
  ENTER_KEY,
  DEL_KEY,
  SPACE_KEY,
  REDRAW_KEY,
};

enum compstate {
  COMP_NONE,
  COMP_LOGIN,
  COMP_TRYAGAIN,
  COMP_AUTHED,
  COMP_HELP,
  COMP_ENFORCE,
  COMP_CEASE,
  COMP_DEBUG,
  COMP_SHUTDOWN,
  COMP_INVALID,
  COMP_CRASH,
  COMP_CRASHTRY,
};

class MainFrameRoom : public Room {
  public:
  MainFrameRoom();
  ~MainFrameRoom(){};

  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void touchDown(int x, int y);
  void touchUp(int x, int y);

  private:
    void reset();
    void doCommand(char* command);
  
    char line[32];
    int Idx;
    keyk kk;
    int key[2];
    bool out;
    compstate state, laststate;
};



#endif

