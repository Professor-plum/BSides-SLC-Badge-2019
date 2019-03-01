#ifndef Adafruit_ILI9341_STM_Ext_Sleep_h
#define Adafruit_ILI9341_STM_Ext_Sleep_h

#include <Adafruit_ILI9341_STM.h>

class Adafruit_ILI9341_STM_Ext_Sleep : public Adafruit_ILI9341_STM {
  using Adafruit_ILI9341_STM::Adafruit_ILI9341_STM;
  public:
  void enterSleepMode() {writecommand(ILI9341_SLPIN);};
  void exitSleepMode() {writecommand(ILI9341_SLPOUT);};
};

#endif
