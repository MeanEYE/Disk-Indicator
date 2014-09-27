#include <linux/kd.h>

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

ConsoleConfig *console_config;

char console_init(int argc, const char *argv[]);
void console_quit();
char console_turn_on();
char console_turn_off();

#endif
