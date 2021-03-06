#pragma once
#define XORG_AVAILABLE

#include <stdbool.h>
#include <X11/Xlib.h>
#include "shared.h"


enum xorg_led {
	XORG_CAPS_LOCK = 1,
	XORG_NUM_LOCK = 2,
	XORG_SCROLL_LOCK = 3
};


typedef struct {
	Display *display;
	XKeyboardState *keyboard_state;

	bool initial_state;
	enum xorg_led flag_bit;
} XOrgConfig;


void xorg_init(Indicator *indicator, char *led);
void xorg_quit(Indicator *indicator);
void xorg_turn_on(Indicator *indicator);
void xorg_turn_off(Indicator *indicator);
