#include <stdbool.h>
#include <X11/Xlib.h>

#ifndef XORG_INCLUDED
#define XORG_INCLUDED

#define X_MASK_CAPS_LOCK 0x1
#define X_MASK_NUM_LOCK 0x2
#define X_MASK_SCROLL_LOCK 0x4

typedef struct {
	Display *display;
	XKeyboardState *keyboard_state;

	bool initial_state;
	char flag_bit;
} XOrgConfig;

XOrgConfig *xorg_config;

char xorg_init(int argc, const char *argv[]);
void xorg_quit();
char xorg_turn_on();
char xorg_turn_off();

#endif
