#ifndef PuzzleRoom_h
#define PuzzleRoom_h

#include "Room.h"
#include "FlashReader.h"

const char pkey[] = "forgotten";

class PuzzleRoom: public Room {
public:
  PuzzleRoom();
  ~PuzzleRoom() {};
  void refresh(Adafruit_ILI9341_STM *tft, unsigned long now);
  roomID update(Adafruit_ILI9341_STM *tft, unsigned long now);
  
  void touchDown(int x, int y);
  void touchMove(int x, int y);
  void touchUp(int x, int y);
  private:
    bool validmove(int x1, int y1, int x2, int y2);
    int tiles[4][4];
    int sel[2];
    bool out;
};

PuzzleRoom::PuzzleRoom() {
  sel[0] = sel[1] = -1;
  tiles[0][0] = 0;
  tiles[0][1] = 1;
  tiles[0][2] = 2;
  tiles[0][3] = 3;
  tiles[1][0] = 4;
  tiles[1][1] = 5;
  tiles[1][2] = 6;
  tiles[1][3] = 7;
  tiles[2][0] = 8;
  tiles[2][1] = 9;
  tiles[2][2] = 10;
  tiles[2][3] = 11;
  tiles[3][0] = 12;
  tiles[3][1] = 13;
  tiles[3][2] = 14;
  tiles[3][3] = -1;
}

void PuzzleRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  out = false;
  FR.blt("puzzlet.raw", tft, 0, 0, 240, 40);
  FR.blt("puzzleb.raw", tft, 0, 280, 240, 40);
}

roomID PuzzleRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  bool solved=true;
  for (int x=0; x<4; ++x)
    for (int y=0; y<4; ++y) {
      if (((x<3) || (y<3)) && tiles[x][y] != x+y*4)
          solved = false;
      if (tiles[x][y] != -1) {
        char filename[12];
        sprintf(filename, "puzzle%d.raw", tiles[x][y]);
        FR.blt(filename, tft, x*60, y*60 + 40, 60, 60); 
      }
      else {
        tft->fillRect(x*60, y*60 + 40, 60, 60, tft->color565(87,11,81));
      }
    }
  if (solved) {
    wheel_set(C_PUZZLE, true);
  }
  if (wheel_get(C_PUZZLE)) {
    char ii = 0;
    if ((now % 800) < 600)
      ii = pkey[(now / 800) % 10];
    for (int i=0; i<8; ++i) {
      uint16_t c;
      if (bitRead(ii, i))
        c = ILI9341_RED;
      else
        c = tft->color565(82, 88, 88);
      tft->fillRect(95, 1+i*5, 5, 3, c);
    }
  }
  if (out) return R_ARCADE;
  return R_NOCHANGE;
}

void PuzzleRoom::touchDown(int x, int y) {
  if ((y < 40) || (y > 280)){
    sel[0] = sel[1] = -1;
  }
  sel[0] = x/60;
  sel[1] = (y-40)/60;
  if ((x<40) && (y>280))
    out = true;
}

void PuzzleRoom::touchMove(int x, int y) {
  int at[2];

  if ((y < 40) || (y > 280)){
    at[0] = at[1] = -1;
  }
  at[0] = x/60;
  at[1] = (y-40)/60;

  if (validmove(sel[0], sel[1], at[0], at[1])) {
    int t = tiles[sel[0]][sel[1]];
    tiles[sel[0]][sel[1]] = tiles[at[0]][at[1]];
    tiles[at[0]][at[1]] = t;
  }

  sel[0] = at[0];
  sel[1] = at[1];
}

void PuzzleRoom::touchUp(int x, int y) {
}

bool PuzzleRoom::validmove(int x1, int y1, int x2, int y2) {
  if ((x1 == -1) || (y1 == -1) || (x2 == -1) || (y2 == -1))
    return false;

  if (tiles[x2][y2] != -1)
    return false;

  if ((x1 == x2) && (abs(y2 - y1) == 1))
    return true;

  if ((y1 == y2) && (abs(x2 - x1) == 1))
    return true;

  return false;
}
#endif

