#ifndef LEDWHEEL
#define LEDWHEEL
#include <Arduino.h>

#define ledTimer	Timer2
const char ledMap[12] = {PB3, PB4, PB5, PB7, PB8, PB9, PC13, PB10, PB11, PB12, PB13, PB14};

void wheel_init();
void wheel_set(int id, bool on);
bool wheel_get(int id);

#endif