#include "GameRoom.h"
#include <LedWheel.h>

const uint8_t baddiesBit[10][24] = {
  { 0x0, 0x0, 0x0, 0x0, 0x2, 0x40, 0x1, 0x80, 0xb, 0xd0, 0xd, 0xb0, 0xf, 0xf0, 0xb, 0xd0, 0x9, 0x90, 0xc, 0x30, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x2, 0x40, 0x7, 0xe0, 0xd, 0xb0, 0xf, 0xf0, 0xa, 0x50, 0x9, 0x90, 0x8, 0x10, 0xc, 0x30, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x40, 0x7, 0xe0, 0xf, 0xf0, 0x1d, 0xb8, 0x17, 0xe8, 0x12, 0x48, 0x6, 0x60, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x4, 0x20, 0x2, 0x40, 0x7, 0xe0, 0xd, 0xb0, 0x1f, 0xf8, 0x17, 0xe8, 0x12, 0x48, 0x4, 0x20, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x1f, 0xf8, 0x3f, 0xfc, 0x39, 0x9c, 0x3f, 0xfc, 0x6, 0x60, 0xd, 0xb0, 0x30, 0xc, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x3, 0xc0, 0x1f, 0xf8, 0x3f, 0xfc, 0x39, 0x9c, 0x3f, 0xfc, 0x6, 0x60, 0x9, 0x90, 0x4, 0x20, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x1, 0x80, 0x3, 0xc0, 0x7, 0xe0, 0xd, 0xb0, 0xf, 0xf0, 0x2, 0x40, 0x4, 0x20, 0x2, 0x40, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x1, 0x80, 0x3, 0xc0, 0x7, 0xe0, 0xd, 0xb0, 0xf, 0xf0, 0x2, 0x40, 0x5, 0xa0, 0xa, 0x50, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x8, 0x10, 0x24, 0x24, 0x2f, 0xf4, 0x3b, 0xdc, 0x3f, 0xfc, 0xf, 0xf0, 0x8, 0x10, 0x10, 0x8, 0x0, 0x0, 0x0, 0x0},
  { 0x0, 0x0, 0x0, 0x0, 0x8, 0x10, 0x4, 0x20, 0xf, 0xf0, 0x1b, 0xd8, 0x3f, 0xfc, 0x2f, 0xf4, 0x28, 0x14, 0x6, 0x60, 0x0, 0x0, 0x0, 0x0},
};

const uint8_t _ship[] = {0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x0, 0x80, 0x0, 0x0, 0x1, 0xc0, 
0x0, 0x0, 0x9, 0xc8, 0x0, 0x0, 0x9, 0xc8, 0x0, 0x0, 0xb, 0xe8, 0x0, 0x0, 0x4f, 
0xf9, 0x0, 0x0, 0x4f, 0xf9, 0x0, 0x0, 0x4f, 0xf9, 0x0, 0x0, 0x5f, 0xfd, 0x0, 0x0, 
0x7f, 0xff, 0x0, 0x0, 0x77, 0xf7, 0x0, 0x0, 0x66, 0xb3, 0x0, 0x0, 0x40, 0x81, 0x0};

uint16_t _colors [5]={ILI9341_NAVY, ILI9341_DARKGREEN, ILI9341_DARKCYAN, ILI9341_MAROON, ILI9341_PURPLE};

void GameRoom::reset() {
  _startTime = millis();
  _lastUpdate = millis();
  _lives = 3;
  _playerX = 120;
  _targetX = _playerX;
  _pIdx = _bIdx = 0;
  _shooting = false;
  _lastShot = 0;
  _bx = 0;
  _by = 0;
  _right = true;
  n = R_NOCHANGE;
  for (int i=0; i<10; ++i)
    for(int j=0; j<5; ++j)
      _baddies[i][j] = true;
  _baddieShot[0] = 0;
  _baddieShot[1] = 400;
  for (int i=0; i<6; ++i)
    for (int j=0; j< 2; ++j) {
      _playerShots[i][j] = -1;
    }
}

void GameRoom::refresh(Adafruit_ILI9341_STM *tft, unsigned long now) {
  tft->fillScreen(ILI9341_BLACK);
  update(tft, now);
}
  
roomID GameRoom::update(Adafruit_ILI9341_STM *tft, unsigned long now) {
  if (now > (_lastUpdate + 5000)) {
    tft->fillScreen(ILI9341_BLACK);
    reset();
  }

  //if ((now % 10000) < 100) tft->fillScreen(ILI9341_BLACK); //garbage collect

  tft->drawFastHLine(0, 303, 240, ILI9341_LIGHTGREY);
  tft->drawFastHLine(0, 304, 240, ILI9341_BLACK);
  tft->fillRect(0, 0, 320, 2, ILI9341_BLACK);
  for(int i=0; i< 10; ++i) {
    if (i< _lives)
      tft->drawBitmap(i*32, 305, _ship, 32, 15, ILI9341_DARKGREY, ILI9341_BLACK);
    else
      tft->fillRect(i*32, 305, 32, 15, ILI9341_BLACK);
  }

  if (now < (_startTime + 2500)) { //Ready screen 
    tft->setCursor(80, 150);
    tft->setFont();
    tft->setTextColor(ILI9341_WHITE);
    tft->setTextSize(2);
    tft->print("Ready");
    tft->setTextSize(1);
  }
  else if (now < (_startTime + 3000)) { //Ready screen 
    tft->fillRect(80, 150, 80, 16, ILI9341_BLACK);
  }
  else if (isWon()) {  // You Won!
    tft->fillRect(80, 150, 140, 20, ILI9341_BLACK);
    tft->setCursor(80, 150);
    tft->setFont();
    tft->setTextColor(ILI9341_WHITE);
    tft->setTextSize(2);
    tft->print("You Won!");
    wheel_set(C_GAME, true);
    tft->setTextSize(1);
  }
  else if (_lives < 0) { //Game Over
    tft->fillRect(80, 150, 140, 20, ILI9341_BLACK);
    tft->setCursor(80, 150);
    tft->setFont();
    tft->setTextColor(ILI9341_WHITE);
    tft->setTextSize(2);
    tft->print("Game Over");
    tft->setTextSize(1);
  }
  else {
    doLogic(tft, now);

    int z = (now/1000) %2;

    //tft->fillRect(0, _by-2, 240, 120, ILI9341_BLACK);
    for (int i=0; i<10; ++i)
      for(int j=0; j<5; ++j) {
        if (_baddies[i][j])
          tft->drawBitmap(_bx + i*16, _by + j*12, baddiesBit[2*j + z], 16, 12, _colors[j], ILI9341_BLACK);
      }

    for (int i=0; i<6; ++i)
      if (_playerShots[i][0] != -1) {
          tft->drawFastVLine(_playerShots[i][0], _playerShots[i][1]+8, 8, ILI9341_BLACK);
          tft->drawFastVLine(_playerShots[i][0], _playerShots[i][1], 8, ILI9341_YELLOW);
      }

    tft->drawBitmap(_playerX-8, 287, _ship, 32, 15, ILI9341_DARKGREY, ILI9341_BLACK);

    tft->fillRect(_baddieShot[0], _baddieShot[1]-8, 2, 8, ILI9341_BLACK);
    tft->fillRect(_baddieShot[0], _baddieShot[1], 2, 8, ILI9341_CYAN);

    //tft->drawBitmap(0, 0, _exitBit, 16, 16);
    
  }
  return n;
}
  
void GameRoom::touchDown(int x, int y) {
  _targetX = x;
  _shooting = true;
  if ((x<20) && (y<20))
    n = R_ARCADE;
}

void GameRoom::touchMove(int x, int y) {
  _targetX = x;
}

void GameRoom::touchUp(int x, int y) {
  _shooting = false;
  _targetX = _playerX;
}

bool GameRoom::isWon() {
  if (_lives < 0)
    return false;
  for (int i=0; i<10; ++i)
    for (int j=0; j<5; ++j)
      if (_baddies[i][j])
        return false;
  return true;
}
    
void GameRoom::doLogic(Adafruit_ILI9341_STM *tft, uint32_t now) {

  int diff = _targetX - _playerX;
  _playerX += constrain(diff, -4, 4);

  _baddieShot[1] += 6;
  if (hit(_baddieShot[0]+2, _baddieShot[1], 3, 7, _playerX, 287, 16, 16)) {
    _lives--;
    tft->fillCircle(_playerX+8, 286, 8, ILI9341_ORANGE);
    tft->fillRect(_playerX-8, 270, 32, 32, ILI9341_BLACK);
    wavPlay("die.wav");
    //TODO: invuln period
    _baddieShot[1]=400;
  }
       
    
  for (int i=0; i<6; ++i)
    if (_playerShots[i][0] > -1) {
      _playerShots[i][1] -= 6;
      if (_playerShots[i][1] < -16)
        _playerShots[i][0] = -1;      
    }

  if (_lastUpdate + 300 < now) {
    if (_baddieShot[1] >= 310) {
          int i = random(10);
          _baddieShot[0] = _bx + i*16 + 6;
          for (int j = 0; j<5; ++j)
            if (_baddies[i][j])
              _baddieShot[1] = _by + (j+1)*12;
    }
    _lastUpdate += 300;
  }
  
  if ((_by % 8) != 0) _by += 2;
  else if (_right) {
    if (_bx >= 80) {
      _right = false;
      _by += 2;
    }
    else 
      _bx += 2;
  }
  else {
    if (_bx <= 0) {
      _right = true;
      _by += 2;
    }
    else 
      _bx -= 2;
  }

  if (_by > 256) _lives=-1;

  
  if ((_shooting) && (now > (_lastShot + 1000))) {
    _lastShot = now;
    _playerShots[_pIdx][0] = _playerX + 8;
    _playerShots[_pIdx++][1] = 288;
    wavPlay("shoot.wav");
    if (_pIdx >= 6) _pIdx = 0;
  }
  
  for (int i=0; i<10; ++i)
    for (int j=0; j<5; ++j) 
      if (_baddies[i][j])
        for (int k=0; k<6; ++k) 
          //_bx + i*16, _by + j*12, baddiesBit[2*j + z], 16, 12, 
          if (hit(_playerShots[k][0], _playerShots[k][1], 1, 8, _bx + i*16 + 2, _by + j*12 +2, 10, 6)) {
            tft->fillCircle(_bx + i*16 + 7, _by + j*12 +5, 3, ILI9341_ORANGE);
            tft->fillRect(_bx + i*16, _by + j*12, 16, 22, ILI9341_BLACK);
            _baddies[i][j] = false;
            _playerShots[k][0] = -1;
            wavPlay("killed.wav");
          }

  for (int i=0; i<10; ++i)
    for (int j=0; j<5; ++j)
      if (_baddies[i][j])
        if (hit(_playerX, 287, 16, 16, _bx + i*16 + 2, _by + j*12 +2, 10, 6)) {
          tft->fillCircle(_playerX+8, 286, 8, ILI9341_ORANGE);
          tft->fillRect(_playerX-8, 270, 32, 32, ILI9341_BLACK);
          _lives--;
          wavPlay("die.wav");
          _baddies[i][j] = false;
          //TODO: invuln period
        }
}


bool GameRoom::hit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  if ((x1 + w1) < x2) return false;
  if ((x2 + w2) < x1) return false;
  if ((y1 + h1) < y2) return false;
  if ((y2 + h2) < y1) return false;
  return true;
}


