/**
 * Support for notifications by keyboard light through
 * X.Org display server. This is a prefered method for
 * users in graphical desktop environment.
 */

#include <stdlib.h>
#include <X11/Xlib.h>
#include "xorg.h"

/**
 * Initialize X.Org display support.
 */
char xorg_init(int argc, const char *argv[])
{
	char result = 0;

	// allocate memory to store config
	xorg_config = malloc(sizeof(XOrgConfig));

	// open connection to display server
	xorg_config->display = XOpenDisplay(":0");
	xorg_config->keyboard_state = malloc(sizeof(XKeyboardState));
	xorg_config->flag_bit = X_MASK_SCROLL_LOCK;

	return result;
}

/**
 * Free memory taken by X.Org display support config.
 */
void xorg_quit()
{
	free(xorg_config->keyboard_state);
	free(xorg_config);
}

/**
 * Turn keyboard LED on.
 */
char xorg_turn_on()
{
	char result = 0;
	unsigned long led_mask;
	XKeyboardControl values;

	// get current LED state
	XGetKeyboardControl(xorg_config->display, xorg_config->keyboard_state);
	led_mask = xorg_config->keyboard_state->led_mask;

	// store initial flag state
	xorg_config->initial_state = (led_mask & xorg_config->flag_bit) == xorg_config->flag_bit;

	// toggle flag
	led_mask ^= xorg_config->flag_bit;

	// set new state
	values.led_mode = LedModeOn;
	values.led = xorg_config->flag_bit;

	XChangeKeyboardControl(xorg_config->display, KBLed | KBLedMode, &values);

	return result;
}

/**
 * Turn keyboard LED off.
 */
char xorg_turn_off()
{
	char result = 0;

	return result;
}
