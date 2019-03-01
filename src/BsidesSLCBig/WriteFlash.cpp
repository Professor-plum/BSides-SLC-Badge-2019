#include <SerialFlash.h>
#include <Adafruit_ILI9341_STM.h>

//Buffer sizes
#define USB_BUFFER_SIZE      128
#define FLASH_BUFFER_SIZE    1024

//Max filename length (8.3 plus a null char terminator)
#define FILENAME_STRING_SIZE      13

//State machine
#define STATE_START      0
#define STATE_SIZE      1
#define STATE_CONTENT    2

//Special bytes in the communication protocol
#define BYTE_START      0x7e
#define BYTE_ESCAPE      0x7d
#define BYTE_SEPARATOR    0x7c

#define ILI9341_VSCRSADD 0x37

//SPI Pins (these are the values on the Audio board; change them if you have different ones)
#define CSPIN              PA4

#define LOG(x) {newline(tft);Serial.print(x);tft->print(x);}

void newline(Adafruit_ILI9341_STM *tft) {
  int c=8;
  uint16_t row[240];
  for (int y=c; y<320; ++y) {
    tft->readPixels(0,y,239,y,row);
    tft->setAddrWindow(0,y-c,240,y-c+1);
    tft->pushColors(row, 240);
  }
  tft->fillRect(0,320-c,240,c,ILI9341_BLACK);
  tft->setCursor(0, 320-c);
}

void lock() {
  while(true);
}

void WriteFlash(Adafruit_ILI9341_STM *tft) {
  tft->setTextColor(ILI9341_WHITE);

  LOG("Erasing Flash Chip...");
  uint8_t id[5];
  SerialFlash.readID(id);
  SerialFlash.eraseAll();
  while (!SerialFlash.ready()) delay(500);
  LOG("Ready to write files");
  while(!Serial.available());

  SerialFlashFile flashFile;
  
  uint8_t state = STATE_START;
  uint8_t escape = 0;
  uint8_t fileSizeIndex = 0;
  uint32_t fileSize = 0;
  char filename[FILENAME_STRING_SIZE];
  
  char usbBuffer[USB_BUFFER_SIZE];
  uint8_t flashBuffer[FLASH_BUFFER_SIZE];
  
  uint16_t flashBufferIndex = 0;
  uint8_t filenameIndex = 0;
  
  uint32_t lastReceiveTime = millis();
  
  //We assume the serial receive part is finished when we have not received something for 3 seconds
  while(Serial.available() || lastReceiveTime + 3000 > millis()){
    uint16_t available = Serial.readBytes(usbBuffer, USB_BUFFER_SIZE);
    if (available){
      lastReceiveTime = millis();
    }

    for (uint16_t usbBufferIndex = 0; usbBufferIndex < available; usbBufferIndex++){
      uint8_t b = usbBuffer[usbBufferIndex];
      
      if (state == STATE_START){
        //Start byte.  Repeat start is fine.
        if (b == BYTE_START){
          for (uint8_t i = 0; i < FILENAME_STRING_SIZE; i++){
            filename[i] = 0x00;
          }
          filenameIndex = 0;
        }
        //Valid characters are A-Z, 0-9, comma, period, colon, dash, underscore
        else if ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') ||(b >= '0' && b <= '9') || b == '.' || b == ',' || b == ':' || b == '-' || b == '_'){
          filename[filenameIndex++] = b;
          if (filenameIndex >= FILENAME_STRING_SIZE){
            LOG("Error: file name too long!");
            lock();
          }
        }
        //Filename end character
        else if (b == BYTE_SEPARATOR){
          if (filenameIndex == 0){
            LOG("Error: Empty filename");
            lock();
          }
          
          //Change state
          state = STATE_SIZE;
          fileSizeIndex = 0;
          fileSize = 0;
          
        }
        //Invalid character
        else {
          LOG("Error: Invalid filename ");
          tft->print(filename);
          lock();
        }
      }
      //We read 4 bytes as a uint32_t for file size
      else if (state == STATE_SIZE){
        if (fileSizeIndex < 4){
          fileSize = (fileSize << 8) + b;
          fileSizeIndex++;
        }
        else if (b == BYTE_SEPARATOR){
          state = STATE_CONTENT;
          flashBufferIndex = 0;
          escape = 0;
          
          if (SerialFlash.exists(filename)){
            SerialFlash.remove(filename);  //It doesn't reclaim the space, but it does let you create a new file with the same name.
          }
          
          //Create a new file and open it for writing
          if (SerialFlash.create(filename, fileSize)) {
            flashFile = SerialFlash.open(filename);
            if (!flashFile) {
              LOG("Unable to open ");
              tft->print(filename);
              lock();
            }
            LOG(fileSize);
            tft->print(" byte file ");
            tft->print(filename);
          }
          else {
            LOG("Cannot create file ");
            tft->print(filename);
            tft->print("out of space?");
            lock();
          }
        }
        else {
          LOG("Invalid file size");
          lock();
        }
      }
      else if (state == STATE_CONTENT){
        //Previous byte was escaped; unescape and add to buffer
        if (escape){
          escape = 0;
          flashBuffer[flashBufferIndex++] = b ^ 0x20;
        }
        //Escape the next byte
        else if (b == BYTE_ESCAPE){
          escape = 1;
        }
        //End of file
        else if (b == BYTE_START){
          state = STATE_START;
          if (flashBufferIndex)
            flashFile.write(flashBuffer, flashBufferIndex);
          flashFile.close();
          flashBufferIndex = 0;
        }
        //Normal byte; add to buffer
        else {
          flashBuffer[flashBufferIndex++] = b;
        }
        
        //The buffer is filled; write to SD card
        if (flashBufferIndex >= FLASH_BUFFER_SIZE){
          flashFile.write(flashBuffer, FLASH_BUFFER_SIZE);
          flashBufferIndex = 0;
        }
      }
    }
  }
  LOG("Upload finished");
  delay(3000);
}




