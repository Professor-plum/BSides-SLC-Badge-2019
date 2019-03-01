#include "LedWheel.h"
#include <EEPROM.h>

bool ledStatus[12] = {0};

void wheel_begin();
void wheel_set(int id, bool on);
bool wheel_get(int id);


void ledinterrupt(void) {
	static int led_idx = 0;
	int oldp = led_idx++%12;
	int newp = led_idx%12;
	digitalWrite(ledMap[oldp], LOW);
	if (ledStatus[newp])
		digitalWrite(ledMap[newp], HIGH);
}

void wheel_setPeriod() {
	int tally=25;
	for (int i=0; i<12; ++i)
		tally += ledStatus[i]?0:15;

	ledTimer.setPeriod(tally * 500);
	ledTimer.refresh();
}

void wheel_init() {
	for (int i=0; i<12; ++i)
		pinMode(ledMap[i], OUTPUT);

	for (int i=0; i<12; ++i)
		ledStatus[i] = EEPROM.read(20+i)==1?true:false;

	ledTimer.pause();
  	ledTimer.setMode(1, TIMER_OUTPUT_COMPARE);
  	ledTimer.setCompare(TIMER_CH1, 1);
  	ledTimer.attachInterrupt(1, ledinterrupt);
  	wheel_setPeriod();
 	ledTimer.resume();
}

void wheel_set(int id, bool on) {
	id %= 12;
	if (ledStatus[id] != on) {
		ledStatus[id]=on;
		EEPROM.update(20+id, (on?1:0));
		wheel_setPeriod();
	}
}

bool wheel_get(int id) {
	return ledStatus[id%12];
}

bool wheel_complete() {
	for (int i=0; i<12; ++i) {
		if (!ledStatus[i])
			return false;
	}
	return true;
}
