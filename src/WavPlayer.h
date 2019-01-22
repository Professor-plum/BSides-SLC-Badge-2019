#ifndef WAVPLAYER_H
#define WAVPLAYER_H

#define USE_FLASH
#define BUZZ      PB6 

bool wavPlay(char* filename);
void wavStop();
bool wavIsPlaying();
void wavCheckBuff();

void wavInit();

#endif