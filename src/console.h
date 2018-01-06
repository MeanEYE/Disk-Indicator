#pragma once
#define CONSOLE_AVAILABLE

#include <linux/kd.h>
#include "shared.h"

#define CONSOLE_CONFIG_FORMAT "%[^|]|%s"


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


void console_init(Indicator *indicator, char *config);
void console_quit(Indicator *indicator);
void console_turn_on(Indicator *indicator);
void console_turn_off(Indicator *indicator);
