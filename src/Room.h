#ifndef Room_h
#define Room_h

#include <Arduino.h>
#include <Adafruit_ILI9341_STM.h>
#include <WavPlayer.h>
#include <LedWheel.h>

enum roomID {
  R_TV,
  R_MARKET,
  R_DIAL,
  R_MAINFRAME,
  R_STREET,
  R_NIGHTCLUB,
  R_DJ,
  R_ARCADE,
  R_GAME,
  R_PUZZLE,
  R_ALLEY,
  R_TOWER,
  R_KEYPAD,
  R_PANEL,
  R_SPLASH,
  R_END,
  R_NOCHANGE,
  R_CONFIGURE
};

enum check {
  C_KEYPAD,  //Medium-1
  C_PASSWORD,  //Medium-1
  C_CODE,  //Hard
  C_PUZZLE, //Hard
  C_CRYPTO,  //Medium-2
  C_SENSOR,  //Easy 
  C_GAME,   //Easy
  C_TV,  //Easy
  C_DJ,  //Medium-1
  C_SOLDER, //Hard
  C_SOCIAL,  //Medium-1
  C_PHONE, //Medium-2
};

class Room
{
public:
  Room() {};
  ~Room() {};

  virtual void refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {};
  virtual roomID update(Adafruit_ILI9341_STM *tft, unsigned long now) {return R_NOCHANGE;};
  
  virtual void touchDown(int x, int y) {};
  virtual void touchMove(int x, int y) {};
  virtual void touchUp(int x, int y) {};
};

#endif

