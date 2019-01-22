#ifndef MainFrameRoom_h
#define MainFrameRoom_h

#include "Room.h"
#include "FlashReader.h"


class MainFrameRoom : public Room {
  public:
  MainFrameRoom();
  ~MainFrameRoom() {};

  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void touchDown(int x, int y);

  private:
    void reset();
    void doCommand(char* command);
  
    char line[32];
    int Idx, tries;
    uint32_t bootat;
    bool authed, booted, conn, crashed, out;
};



#endif

