#ifndef GameRoom_h
#define GameRoom_h

#include "Room.h"

class GameRoom: public Room {
  public:
  GameRoom(): _lastUpdate(0), n(R_NOCHANGE) {reset();};
  ~GameRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);

  void reset();
  
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  void touchUp(int x, int y);
  private:
    bool isWon();
    void doLogic(Adafruit_ILI9341_STM *tft, uint32_t now);
    static bool hit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

    uint16_t _upInt;
    uint32_t _startTime, _lastUpdate, _lastShot;
    int _lives, _playerX, _targetX;
    int _bx, _by;
    int _pIdx, _bIdx;
    bool _baddies[10][5], _shooting, _right;
    int _playerShots[6][2];
    int _baddieShot[2];
    roomID n;
};

#endif

