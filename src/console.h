#include <linux/kd.h>
#include "shared.h"

#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

enum console_led {
	SCROLL_LOCK = LED_SCR,
	NUM_LOCK = LED_NUM,
	CAPS_LOCK = LED_CAP
};

typedef struct {
	int device;
	enum console_led led;
	char tty[100];
} ConsoleConfig;


void console_init(Indicator *indicator, char *device);
void console_quit();
void console_turn_on();
void console_turn_off();

#endif
