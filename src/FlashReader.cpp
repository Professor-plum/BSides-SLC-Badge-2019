#include "FlashReader.h"

void FlashReader::blt(const char* filename, Adafruit_ILI9341_STM *tft, int offx, int offy, int w, int h) {
  SerialFlashFile flashFile  = SerialFlash.open(filename);
  if (!flashFile) {
    Serial.print(filename); Serial.println(F(" File not found"));
    return;
  }
  uint16_t buf[240];

  for(int y=0; y<h; ++y) {
    int x = flashFile.read(buf, 2*w);
    if (x != 2*w) Serial.println("Couldn't read from file!");
    SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
    tft->setAddrWindow(offx, offy+y, offx+w-1, offy+y);
    tft->pushColors(buf, w);
  }
  flashFile.close();

}

FlashReader FR;

