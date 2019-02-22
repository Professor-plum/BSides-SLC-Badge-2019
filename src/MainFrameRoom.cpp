#include "MainFrameRoom.h"
#include <SerialFlash.h>


const unsigned long shash = 3928239000;

const char keymap[3][11] = {
  "QWERTYUIOP",
  "ASDFGHJKL ",
  "ZXCVBNM   "
};

const uint8_t shell_code[] = { 
  0x55, 0x89, 0xE5, 0x68, 0x63, 0x6E, 0x72, 0x79, 0x83, 0xEC, 
  0x10, 0xB8, 0x03, 0x00, 0x00, 0x00, 0x31, 0xDB, 0x89, 0xE1, 
  0xBA, 0x10, 0x00, 0x00, 0x00, 0xCD, 0x80, 0xE8, 0x1E, 0x9A, 
  0x54, 0x30, 0x83, 0xC4, 0x10, 0x59, 0x81, 0xF9, 0x63, 0x6E, 
  0x72, 0x79, 0x74, 0x0E, 0xB8, 0x25, 0x00, 0x00, 0x00, 0x31, 
  0xDB, 0xB9, 0x0B, 0x00, 0x00, 0x00, 0xCD, 0x80, 0x89, 0xEC, 
  0x5D, 0xC3, 0x66, 0x90 };


const char* debug[] = 
   { "<cmdin>:    push %ebp",
   "<cmdin+1>:  mov  %esp,%ebp",
   "<cmdin+3>:  sub  $0x28,%esp",
   "<cmdin+6>:  movl $0x59524e43,-0xc(%ebp)",
   "<cmdin+13>: sub  $0x4,%esp",
   "<cmdin+16>: push $0x400",
   "<cmdin+21>: lea  -0x1c(%ebp),%eax",
   "<cmdin+24>: push %eax",
   "<cmdin+25>: push $0x0",
   "<cmdin+27>: call <read>",
   "<cmdin+32>: add  $0x10,%esp",
   "<cmdin+35>: sub  $0xc,%esp",
   "<cmdin+38>: lea  -0x1c(%ebp),%eax",
   "<cmdin+41>: push %eax",
   "<cmdin+42>: call 0x11b9 <do_cmd>",
   "<cmdin+47>: add  $0x10,%esp",
   "<cmdin+50>: cmpl $0x59524e43,-0xc(%ebp)",
   "<cmdin+57>: je   <cmdin+64>",
   "<cmdin+59>: call <err_ovrflw>",
   "<cmdin+64>: nop",
   "<cmdin+65>: leave",
   "<cmdin+66>: ret" };

unsigned long djhash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

   
MainFrameRoom::MainFrameRoom(){
  reset();
}

const uint16_t cpc[] = {ILI9341_NAVY, ILI9341_DARKGREEN, ILI9341_DARKCYAN, ILI9341_MAROON, ILI9341_PURPLE};

void MainFrameRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  FR.blt("comp.raw", tft, 0, 0, 240, 225);
  kk = REDRAW_KEY;
  if (wheel_get(C_CODE))
    state = COMP_CRASH;
  update(tft, now);
}

roomID MainFrameRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (out) {
     reset();
     return R_MARKET;
  }

  uint16_t cd =  tft->color565(142, 34, 34);

  switch (kk){
    case REDRAW_KEY:
      FR.blt("keyboard.raw", tft, 0, 225, 240, 95);
      break;
    case A_KEY:
      tft->fillRoundRect(key[0]+1, key[1]+1, 23, 23, 1, cd);
      break;
    case ENTER_KEY:
      tft->fillRoundRect(225, 249, 15, 26, 1, cd);
      tft->fillRoundRect(213, 273, 27, 23, 1, cd);
      break;
    case SPACE_KEY:
      tft->fillRoundRect(49, 297, 143, 23, 1, cd);
      break;
    case DEL_KEY:
      tft->fillRoundRect(213, 297, 27, 23, 1, cd);
      break;
  }
  kk=NONE_KEY;

  tft->setTextColor(tft->color565(10, 140, 10), tft->color565(0x42, 0x42, 0x3f));
  tft->setFont();

  if (state != laststate) {
    tft->fillRoundRect(1,3, 237, 194, 3, tft->color565(0x42, 0x42, 0x3f));
    switch(state) {
      case COMP_TRYAGAIN:
        tft->setCursor(2, 52);
        tft->print("          Invalid Password");
      case COMP_LOGIN:
        tft->setCursor(2, 4);
        tft->print("#######################################");
        tft->setCursor(2, 12);
        tft->print("#                                     #");
        tft->setCursor(2, 20);
        tft->print("#  WELCOME TO THE OVERLORD MAINFRAME  #");
        tft->setCursor(2, 28);
        tft->print("#                                     #");
        tft->setCursor(2, 36);
        tft->print("#######################################");
        tft->setCursor(2, 68);
        tft->print("      Please enter your password");
        break;
      case COMP_AUTHED:
        tft->setCursor(2, 20);
        tft->print("Login successful");
        tft->setCursor(2, 28);
        tft->print("system status OKAY");
        break;
      case COMP_HELP:
        tft->setCursor(2, 4);
        tft->print("> HELP");
        tft->setCursor(2, 20);
        tft->print("available commands:");
        tft->setCursor(2, 28);
        tft->print("ENFORCE      starte enforcment project");
        tft->setCursor(2, 36);
        tft->print("CEASE        stops enforcment project");
        tft->setCursor(2, 44);
        tft->print("DEBUG        displays current assembly");
        tft->setCursor(2, 52);
        tft->print("SHUTDOWN     halts mainframe execution");
        break;
      case COMP_ENFORCE:
        tft->setCursor(2, 4);
        tft->print("> ENFORCE");
        tft->setCursor(2, 20);
        tft->print("Peace enforcement program is already");
        tft->setCursor(2, 28);
        tft->print("in operation.");
        break;
      case COMP_CEASE:
        tft->setCursor(2, 4);
        tft->print("> CEASE");
        tft->setCursor(2, 20);
        tft->print("Sorry, I will not comply.");
        break;
      case COMP_DEBUG:
        tft->setCursor(2, 4);
        tft->print("> DEBUG");
        for (int i=0; i< 22; ++i) {
          tft->setCursor(2, 12 + i*8);
          tft->print(debug[i]);
        }
        break;
      case COMP_SHUTDOWN:
        tft->setCursor(2, 4);
        tft->print("> SHUTDOWN");
        tft->setCursor(2, 20);
        tft->print("denied");
        break;
      case COMP_INVALID:
        tft->setCursor(2, 4);
        tft->print("> ");
        tft->print(line);
        tft->setCursor(2, 20);
        tft->print("Execption 11: Segmentation Fault");
        break;
      case COMP_CRASHTRY:
        tft->setCursor(2, 4);
        tft->print("> ");
        tft->print(line);
        tft->setCursor(2, 20);
        tft->print("What do you think you're doing?");
        break;
      case COMP_CRASH:
        tft->setCursor(2, 4);
        tft->print("> ");
        tft->print(line);
        tft->setCursor(2, 20);
        tft->print("Segmentation fault!");
        tft->setCursor(2, 36);
        tft->print("D0: BD8568E7     A0: 0000000000");
        tft->setCursor(2, 44);
        tft->print("D1: 00002020     A1: 4147494C45");
        tft->setCursor(2, 52);
        tft->print("D2: 000169D0     A2: 4452495645");
        tft->setCursor(2, 60);
        tft->print("D3: 00FF01E6     A3: 4D49474854");
        tft->setCursor(2, 68);
        tft->print("D4: 00000234     A4: 4845415254");
        tft->setCursor(2, 76);
        tft->print("D5: 98C00000     A5: 5649474F52");
        tft->setCursor(2, 84);
        tft->print("D6: 98C002BE     A6: 0000000000");
        tft->setCursor(2, 100);
        tft->print("PC: 41414141     SR: 2608");
        tft->setCursor(2, 116);
        tft->print("Execution halted");
        break;
    }
    laststate = state;
    Idx=0;
    memset(line, 0, sizeof(line));
  }

  if ((state > COMP_TRYAGAIN)) {
    if (state != COMP_CRASH) {
      tft->setCursor(3, 188);
      tft->print("> ");
      tft->print(line);
      tft->print("     ");
    }
  }
  else {
    tft->setCursor(3, 188);
    tft->print("PASSWORD: ");
    for (int i=0; i< Idx; ++i) 
      tft->print("*");
    tft->print("     ");
  }

  if (line[Idx-1]=='\r') {
     line[Idx-1] = '\0';
     if (state > COMP_TRYAGAIN) {
        doCommand(line);
     }
     else {
       if (djhash(line)==shash) {
         state = COMP_AUTHED;
         wheel_set(C_PASSWORD, true);
       }
       else {
         state = COMP_TRYAGAIN;
       }
     }
     laststate = COMP_NONE;
  }

  return R_NOCHANGE;
}

void MainFrameRoom::reset() {
  Idx = 0;
  memset(line, 0, sizeof(line));
  kk = NONE_KEY;
  state = COMP_LOGIN;
  laststate = COMP_NONE;
  out = false;
}

void MainFrameRoom::touchDown(int x, int y) {
  if ((x<28) && (y>296)) {
    out = true;
  }
  if (state == COMP_CRASH)
    return;
  if (y>224) {
    int ky = (y-224)/24;
    int kx;
    if (ky == 0) {
      kk = A_KEY;
      kx = x/24;
      line[Idx++] = keymap[ky][kx];
      key[0] = kx *24;
      key[1] = 224 + ky *24;
    }
    else if (ky == 1) {
      if (x>224) {
        kk = ENTER_KEY;
        line[Idx++]='\r';
      }
      else if (x>8) {
        kk = A_KEY;
        kx = (x-8)/24;
        line[Idx++] = keymap[ky][kx];
        key[0] = 8 + kx *24;
        key[1] = 224 + ky *24;
      }
    }
    else if (ky == 2) {
      if (x>212) {
        kk = ENTER_KEY;
        line[Idx++]='\r';
      }
      else if (x>20) {
        kk = A_KEY;
        kx = (x-20)/24;
        line[Idx++] = keymap[ky][kx];
        key[0] = 20 + kx *24;
        key[1] = 224 + ky *24;
      }
    }
    else if (ky == 3) {
      if (x>212) {
        kk = DEL_KEY;
        if (Idx>0) {
          line[--Idx]=' ';
        }
      }
      else if ((x>48) && (x<192)) {
        kk = SPACE_KEY;
        line[Idx++] = ' ';
      }
    }
    if (Idx >= 31) {
      Idx=30;
    }
  }
}

void MainFrameRoom::touchUp(int x, int y) {
  kk = REDRAW_KEY;
}


void MainFrameRoom::doCommand(char* cmd) {
  if (strlen(cmd)==0) {
    state = COMP_AUTHED;
  }
  else if (strlen(cmd) > 15) {
    if ((strlen(cmd) > 20) && (memcmp(cmd+16, "CNRY", 4) == 0)) {
      state = COMP_CRASH;
      wheel_set(C_CODE, true);
    }
    else {
      state = COMP_CRASHTRY;
    }
  }
  else if (strcmp(cmd, "HELP")==0) {
    state = COMP_HELP;
  }
  else if (strcmp(cmd, "CEASE")==0) {
    state = COMP_CEASE;
  }
  else if (strcmp(cmd, "ENFORCE")==0) {
    state = COMP_ENFORCE;
  }
  else if (strcmp(cmd, "SHUTDOWN")==0) {
    state = COMP_SHUTDOWN;
  }
  else if (strcmp(cmd, "DEBUG")==0) {
    state = COMP_DEBUG;
  }
  else {
    state = COMP_INVALID;
  }
}


