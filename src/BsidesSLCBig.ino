#include <SPI.h>
#include <Adafruit_ILI9341_STM.h>
#include <XPT2046_Touchscreen.h>
#include <LedWheel.h>
#include <EEPROM.h>

#include "Room.h"
#include "FlashReader.h"

#include "AlleyRoom.h"
#include "DialRoom.h"
#include "MarketRoom.h"
#include "GameRoom.h"
#include "MainFrameRoom.h"
#include "TVRoom.h"
#include "StreetRoom.h"
#include "ArcadeRoom.h"
#include "TowerRoom.h"
#include "KeypadRoom.h"
#include "PuzzleRoom.h"
#include "NightClub.h"
#include "DJRoom.h"
#include "PanelRoom.h"
#include "SplashRoom.h"


#define TFT_DC    PA2
#define TFT_RST   PB1 
#define TFT_CS    PA3
#define TFT_LED   PB0

#define SF_CS     PA4

#define T_CS      PA1
#define T_IRQ     PA8

#define BUZZ      PB6 

#define MAX_LIGHT 60000

Adafruit_ILI9341_STM tft = Adafruit_ILI9341_STM(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(T_CS, T_IRQ);

uint16_t TS_MINX    = 300;
uint16_t TS_MINY    = 200;
uint16_t TS_MAXX    = 3800;
uint16_t TS_MAXY    = 3600;

Room* roomTable[R_NOCHANGE];
Room *cur;

void calibrate();
bool ask();

void setup() {
  Serial.begin(115200);
  
  pinMode(PA10, INPUT_PULLUP);
  Serial1.begin(9600);
  
  FR.begin(SF_CS);
  
  tft.begin();
  //tft.setRotation(1);
  tft.fillScreen(ILI9341_BLUE);
  
  ts.begin();
  ts.setRotation(3);

  randomSeed(analogRead(PA0));
  EEPROM.init();
  
  peer_id = EEPROM.read(10);
  if ((peer_id <=0) || (peer_id > 5)) {
    peer_id = rand() % 5 +1;
    Serial.print("Invalid peer_id, Resetting to ");Serial.println(peer_id);
    EEPROM.write(10, peer_id);
    calibrate();
  }
  for (int i=0; i< 5; i++) {
    if (EEPROM.read(40+i) == 0xcc)
      peers[i] = true;
  }

  TS_MINX = EEPROM.read(50);
  TS_MINY = EEPROM.read(52);
  TS_MAXX = EEPROM.read(54);
  TS_MAXY = EEPROM.read(56);

  wheel_init();
  wavInit();

  pinMode(LIGHT, INPUT_ANALOG);
  pinMode(NOTPIN, INPUT);

  roomTable[R_ALLEY] = new AlleyRoom();
  roomTable[R_TV] = new TVRoom();
  roomTable[R_MARKET] = new MarketRoom();
  roomTable[R_DIAL] = new DialRoom();
  roomTable[R_MAINFRAME] = new MainFrameRoom();
  roomTable[R_STREET] = new StreetRoom();
  roomTable[R_NIGHTCLUB] = new NightClub();
  roomTable[R_ARCADE] = new ArcadeRoom();
  roomTable[R_GAME] = new GameRoom();
  roomTable[R_DJ] = new DJRoom(); 
  roomTable[R_PUZZLE] = new PuzzleRoom();
  roomTable[R_KEYPAD] = new KeypadRoom();
  roomTable[R_TOWER] = new TowerRoom();
  roomTable[R_PANEL] = new PanelRoom();
  roomTable[R_SPLASH] = new SplashRoom();

  cur =  roomTable[R_SPLASH];
  cur->refresh(&tft, 0);

}

uint32_t last=0;
uint32_t lastDraw=0;
int backlight = MAX_LIGHT;
bool touched = false;
bool sleeping = false;
TS_Point p;
uint32_t lastSerial = 0;

void loop() {
  uint32_t now=millis();
  uint32_t dt = now - last;
  last = now;

  if (sleeping) {
      if (ts.touched()) {
        SerialFlash.wakeup();
        SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
        tft.exitSleepMode();
        sleeping = false;
      }
      return; //early out
    }

  wavCheckBuff();

  if (true){//(now - lastDraw > 100) { //shoot for 10 fps, may not achieve this
    SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
    roomID id = cur->update(&tft, now);
    lastDraw = now;
    if (id == R_CONFIGURE) {
      calibrate();
      if (ask("Do you want to reset progress?")) {
        for (int i=0; i<12; ++i)
          EEPROM.update(20+i, 0);
        EEPROM.update(10, 0xff);
        for (int i=0; i<6; ++i)
          EEPROM.update(40+i, 0);
      }
      if (ask("Do you want to reprogram the flash?"))
        if (ask("Are you sure, this will wipe all data?!"))
          WriteFlash(&tft);
      nvic_sys_reset();
    }
    else if (id != R_NOCHANGE) {
      cur = roomTable[id];
      tft.fillScreen(ILI9341_BLACK);
      cur->refresh(&tft, now);
    }
  }

  wavCheckBuff();
    
  if (ts.touched()) {
    p = ts.getPoint();
    backlight = MAX_LIGHT;
    digitalWrite(TFT_LED, LOW);
    int t = p.x;
    p.x = p.y;
    p.y = t;
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320); 
    if (touched)
      cur->touchMove(p.x, p.y);
    else {
      touched = true;
      cur->touchDown(p.x, p.y);
    }

  }
  else if (touched) {
    touched = false;
    cur->touchUp(p.x, p.y);
  }

  if (lastSerial + 1000 < now) {
    Serial1.write(0xcc);
    Serial1.write(peer_id);
    Serial1.write(0xcc ^ peer_id);
    lastSerial = now;
  }
  while (Serial1.available() >= 3) {
    uint8_t b = Serial1.read();
    if (b == 0xcc) {
      int c = Serial1.read();
      int d = Serial1.read();
      if ( c == (d ^ 0xcc)) {
        if ((c>0) && c<6) {
          Serial.print("Found neew Peer! ");Serial.println(c, HEX);
          if (!peers[--c]) {
            peers[c] = true;
            EEPROM.update(40+c, 0xcc);
          }
        }
      }
    }
  }

  if (backlight > 0) {
    analogWrite(TFT_LED, max(0, 255-(backlight/5)));
    backlight-=dt;
  }
  else { //Tine to sleep
    SerialFlash.sleep();
    SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
    tft.enterSleepMode();
    sleeping = true;
    digitalWrite(TFT_LED, HIGH);
  }

}

void calibrate() {
  int poses[2] = {0,0};
  digitalWrite(TFT_LED, HIGH);
  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
  tft.fillScreen(ILI9341_RED);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(85, 120);
  tft.print("With pen,");
  tft.setCursor(50, 130);
  tft.print("touch top left");
  tft.setCursor(40, 140);
  tft.print("corner of screen");
  tft.drawFastHLine(1, 0, 5, ILI9341_BLACK);
  tft.drawFastVLine(0, 1, 5, ILI9341_BLACK);
  delay(500);

  for (int i=0; i< 10; ++i) {
    while (!ts.touched()) delay(50);
    TS_Point p = ts.getPoint();

    poses[1] += p.x;
    poses[0] += p.y;
  }
  TS_MINX = poses[0]/10;
  TS_MINY = poses[1]/10;
  poses[0] = poses[1] = 0;

  while (ts.touched()) delay(50);

  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
  tft.fillScreen(ILI9341_GREEN);
  tft.setCursor(85, 120);
  tft.print("With pen,");
  tft.setCursor(20, 130);
  tft.print("touch bottom right");
  tft.setCursor(40, 140);
  tft.print("corner of screen");
  tft.drawFastHLine(234, 319, 5, ILI9341_BLACK);
  tft.drawFastVLine(239, 314, 5, ILI9341_BLACK);  
  delay(1000);

  for (int i=0; i< 10; ++i) {
    while (!ts.touched()) delay(50);
    TS_Point p = ts.getPoint();

    poses[1] += p.x;
    poses[0] += p.y;
  }
  TS_MAXX = poses[0]/10;
  TS_MAXY = poses[1]/10; 

  EEPROM.update(50, TS_MINX);
  EEPROM.update(52, TS_MINY);
  EEPROM.update(54, TS_MAXX);
  EEPROM.update(56, TS_MAXY);

  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
  tft.setTextSize(1);
}

bool ask(const char* msg) {
  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
  tft.fillRoundRect(2,100, 236, 100, 5, ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 120);
  tft.setTextSize(1);
  tft.print(msg);
  tft.fillRoundRect(30, 170, 60, 20, 3, ILI9341_NAVY);
  tft.fillRoundRect(150, 170, 60, 20, 3, ILI9341_NAVY);
  tft.setCursor(50, 175);
  tft.print("Yes");
  tft.setCursor(170, 175);
  tft.print("No");
  delay(500);

  while (true) {
    if (ts.touched()) {
      TS_Point p = ts.getPoint();
      int t = p.x;
      p.x = p.y;
      p.y = t;
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320); 
      if ((p.y > 170) && (p.y < 190)){
        if ((p.x>30) && (p.x<90)) return true;
        if ((p.x>150) && (p.x<210)) return false;
      }
    }
    delay(50);
  }
  SPI.beginTransaction(SPISettings(48000000, MSBFIRST, SPI_MODE0, DATA_SIZE_16BIT));
}

