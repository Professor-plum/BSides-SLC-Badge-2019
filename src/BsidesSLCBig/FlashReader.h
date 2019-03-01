#ifndef FlashReader_h
#define FlashReader_h

#include <SerialFlash.h>
#include <Adafruit_ILI9341_STM.h>


class FlashReader {
  public:
    FlashReader() {};
    void begin(int pin) {
      SerialFlash.begin(pin);
      /*unsigned char id[5];
      SerialFlash.readID(id);
      unsigned long size = SerialFlash.capacity(id);
      Serial.print("Flash Memory has ");Serial.print(size);Serial.println(" bytes.");*/
    };
    void blt(const char* filename, Adafruit_ILI9341_STM *tft, int offx, int offy, int w, int h);
};

extern FlashReader FR;

#endif


