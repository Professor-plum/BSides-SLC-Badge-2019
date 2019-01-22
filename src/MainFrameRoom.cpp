#include "MainFrameRoom.h"
#include <SerialFlash.h>
//#include <Fonts/bitwise9pt7b.h>
#include <Fonts/nes.h>

//const char* pass = "maxpower";
unsigned long shash = 1587584152;

const uint8_t shell_code[] = { 
  0x55, 0x89, 0xE5, 0x68, 0x63, 0x6E, 0x72, 0x79, 0x83, 0xEC, 
  0x10, 0xB8, 0x03, 0x00, 0x00, 0x00, 0x31, 0xDB, 0x89, 0xE1, 
  0xBA, 0x10, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xE8, 0x1E, 0x9A, 
  0x54, 0x30, 0x83, 0xC4, 0x10, 0x59, 0x81, 0xF9, 0x63, 0x6E, 
  0x72, 0x79, 0x74, 0x0E, 0xB8, 0x25, 0x00, 0x00, 0x00, 0x31, 
  0xDB, 0xB9, 0x0B, 0x00, 0x00, 0x00, 0xCD, 0x80, 0x89, 0xEC, 
  0x5D, 0xC3, 0x66, 0x90 };


const char* debug = \
   "0x11f0 <input_cmd>:    push   %ebp\n" \
   "0x11f1 <input_cmd+1>:  mov    %esp,%ebp\n" \
   "0x11f3 <input_cmd+3>:  sub    $0x28,%esp\n" \
   "0x11f6 <input_cmd+6>:  movl   $0x79726e63,-0xc(%ebp)\n" \
   "0x11fd <input_cmd+13>: sub    $0x4,%esp\n" \
   "0x1200 <input_cmd+16>: push   $0x400\n" \
   "0x1205 <input_cmd+21>: lea    -0x1c(%ebp),%eax\n" \
   "0x1208 <input_cmd+24>: push   %eax\n" \
   "0x1209 <input_cmd+25>: push   $0x0\n" \
   "0x120b <input_cmd+27>: call   0xf7eb0ff0 <read>\n" \
   "0x1210 <input_cmd+32>: add    $0x10,%esp\n" \
   "0x1213 <input_cmd+35>: sub    $0xc,%esp\n" \
   "0x1216 <input_cmd+38>: lea    -0x1c(%ebp),%eax\n" \
   "0x1219 <input_cmd+41>: push   %eax\n" \
   "0x121a <input_cmd+42>: call   0x11b9 <do_cmd>\n" \
   "0x121f <input_cmd+47>: add    $0x10,%esp\n" \
   "0x1222 <input_cmd+50>: cmpl   $0x79726e63,-0xc(%ebp)\n" \
   "0x1229 <input_cmd+57>: je     0x1230 <input_cmd+64>\n" \
   "0x122b <input_cmd+59>: call   0x11d0 <handle_overflow>\n" \
   "0x1230 <input_cmd+64>: nop\n" \
   "0x1231 <input_cmd+65>: leave\n" \
   "0x1232 <input_cmd+66>: ret\n";
   
MainFrameRoom::MainFrameRoom() {
  crashed = false;
  reset();
}

const uint16_t cpc[] = {ILI9341_NAVY, ILI9341_DARKGREEN, ILI9341_DARKCYAN, ILI9341_MAROON, ILI9341_PURPLE};

void MainFrameRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("computer.raw", tft, 0, 0, 240, 320);
  
  for (int x=112; x<210; x+=6)
    for (int y=82; y<102; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);

  for (int x=194; x<212; x+=6)
    for (int y=222; y<238; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);


  for (int x=32; x<48; x+=6)
    for (int y=222; y<238; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);

  //tft->setFont(&bitwise9pt7b);
  tft->setFont(&Pixel_NES5pt7b);
  tft->setTextColor(ILI9341_GREEN);
  tft->setCursor(24, 16);
  if (crashed)
    tft->print("System error!");
  else {
    tft->print("Modem connection");
    tft->setCursor(24, 32);
    tft->print("established over");
    tft->setCursor(24, 48);
    tft->print("USB Serial");
  }
}

unsigned long djhash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

roomID MainFrameRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  //uint16_t color = ((now / 500) % 2) ? ILI9341_GREEN : ILI9341_BLACK;
  //tft->drawRect(100, 100, 6, 2, color);

  if (out) {
     reset();
     return R_MARKET;
  }

  if (crashed) {
    return R_NOCHANGE;
  }

  for (int x=112; x<210; x+=6)
    for (int y=82; y<102; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);

  for (int x=194; x<212; x+=6)
    for (int y=222; y<238; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);

  for (int x=32; x<48; x+=6)
    for (int y=222; y<238; y+=6)
      tft->fillRect(x, y, 3, 3, cpc[random(5)]);

  if ((bootat>0) && (now > bootat)) {
    reset();
    return R_MARKET;
  }

  if (conn) {
    while (Serial.available()) {
      char c = Serial.read();
      line[Idx++] = c;
      if (Idx >= 32) Idx--;
      if ((c=='\n') || (c=='\r')) {
        line[Idx-1] = '\0';
        if (authed) {
          doCommand(line);
        }
        else {
          if (djhash(line)==shash) {
            authed = true;
            Serial.print("\n\rAccess Granted\n\r>");
            wheel_set(C_PASSWORD, true);
            
            tft->fillRect(24, 8, 200, 48, tft->color565(7,38,12));
            tft->setFont(&Pixel_NES5pt7b); //tft->setFont(&bitwise9pt7b);
            tft->setTextColor(ILI9341_GREEN);
            tft->setCursor(24, 16);
            tft->print("Remote access");
            tft->setCursor(24, 32);
            tft->print("granted");
          }
          else {
            Serial.print("\n\rInvalid Password\n\r");
            if (++tries==3) {
              Serial.print("Access denied!");
              bootat = now + 3000;
            }
            else {
              Serial.print("Please enter your password: ");
            }
          }
        }
        Idx=0;
      }
      else {
        if (authed) Serial.print(c);
        else Serial.print('*');
      }
    }
  }
  if ((!conn) && Serial) {
    conn = true;
    Serial.print("\033[1;31mOverlord Mainframe\n\rPlease enter your password: ");
  }

  return R_NOCHANGE;
}

void MainFrameRoom::reset() {
  booted = false;
  bootat = 0;
  tries = 0;
  authed = false;
  conn = false;
  Idx = 0;
  out = false;
}

void MainFrameRoom::touchDown(int x, int y) {
  if ((x<20) && (y>280)) {
    out = true;
  }
}


void MainFrameRoom::doCommand(char* cmd) {
  Serial.println("");
  if (strlen(cmd)==0) {
  }
  else if (strlen(cmd) > 15) {
    if ((strlen(cmd) > 20) && (memcmp(cmd+16, "cnry", 4) == 0)) {
      Serial.println("Segmentation fault!");
      wheel_set(C_CODE, true);
      crashed=true;
    }
    else {
      Serial.println("What do you think you're doing?");
    }
  }
  else if (stricmp(cmd, "HELP")==0) {
    Serial.println("Available Commands:");
    Serial.println("ENFORCE      Starts peace enforcement project");
    Serial.println("CEASE        Stops peace enforcement project");
    Serial.println("DEBUG        Displays current assembly block");
    Serial.println("LOGOUT       Ends current session");
  }
  else if (stricmp(cmd, "ENFORCE")==0) {
    Serial.print("Peace enforcment program is already in operation\n");
  }
  else if (stricmp(cmd, "CEASE")==0) {
    Serial.print("Sorry, I will not comply\n");
  }
  else if (stricmp(cmd, "LOGOUT")==0) {
    Serial.print("Connection ended\n");
    bootat = millis() + 1000;
    return;
  }
  else if (stricmp(cmd, "DEBUG")==0) {
    /*for (int i=0; i<sizeof(shell_code); i+=16) {
      Serial.print(i, HEX); Serial.print(": ");
      for (int j=0; j<16; ++j) {
        Serial.print(shell_code[j+i], HEX);
        Serial.print(" ");
      }
      Serial.print("\n");
    }*/
    Serial.print(debug);
  }
  else {
    Serial.print("Unknown command: ");Serial.println(cmd);
  }
  Serial.print("\r>");
}


