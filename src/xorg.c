/**
 * Disk Indicator 0.2
 * Copyright © 2015. by Mladen Mijatov <meaneye.rcf@gmail.com>.
 *
 * Support for notifications by keyboard light through
 * X.Org display server. This is a prefered method for
 * users in graphical desktop environment.
 *
 * This nifty little tool will turn your scroll lock (or some other LED)
 * into your disk activity indicator. Program must be started as root.
 * It had very little overhead and almost no CPU usage.
 *
 * Original idea and some code by Dan Stahlke <dan@stahlke.org>. His
 * source code can be found here: http://www.stahlke.org/dan/hdd_led/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <X11/Xlib.h>
#include "xorg.h"

/**
 * Initialize X.Org display support.
 */
void xorg_init(Indicator *indicator, char *led)
{
	// allocate memory to store config
	XOrgConfig *xorg_config = calloc(1, sizeof(XOrgConfig));
	indicator->config = (char *) xorg_config;

	// open connection to display server
	xorg_config->display = XOpenDisplay(NULL);
	xorg_config->keyboard_state = calloc(1, sizeof(XKeyboardState));
	xorg_config->flag_bit = XORG_SCROLL_LOCK;

	// parse additional parameters
	if (strncmp(led, "scroll", 6) == 0) {
		xorg_config->flag_bit = XORG_SCROLL_LOCK;
	} else if (strncmp(led, "num", 3) == 0) {
		xorg_config->flag_bit = XORG_NUM_LOCK;
	} else if (strncmp(led, "caps", 4) == 0) {
		xorg_config->flag_bit = XORG_CAPS_LOCK;
	}

	// mark indicator as initialized
	indicator->initialized = true;
}

/**
 * Free memory taken by X.Org display support config.
 */
void xorg_quit(Indicator *indicator)
{
	XOrgConfig *xorg_config = (XOrgConfig *) indicator->config;

	XCloseDisplay(xorg_config->display);
	free(xorg_config->keyboard_state);
	free(xorg_config);
}

/**
 * Turn keyboard LED on.
 */
void xorg_turn_on(Indicator *indicator)
{
	unsigned long led_mask;
	XKeyboardControl values;

	// get config
	XOrgConfig *xorg_config = (XOrgConfig *) indicator->config;

	// get current LED state
	XGetKeyboardControl(xorg_config->display, xorg_config->keyboard_state);
	led_mask = xorg_config->keyboard_state->led_mask;

	// store initial flag state
	xorg_config->initial_state = (led_mask & xorg_config->flag_bit) == xorg_config->flag_bit;

	// set new state
	if (xorg_config->initial_state)
		values.led_mode = LedModeOff; else
		values.led_mode = LedModeOn;
	values.led = xorg_config->flag_bit;

	XChangeKeyboardControl(xorg_config->display, KBLed | KBLedMode, &values);

	// get the state again to apply changes
	XGetKeyboardControl(xorg_config->display, xorg_config->keyboard_state);
}

/**
 * Turn keyboard LED off.
 */
void xorg_turn_off(Indicator *indicator)
{
	XKeyboardControl values;

	// get config
	XOrgConfig *xorg_config = (XOrgConfig *) indicator->config;

	// set new state
	if (xorg_config->initial_state)
		values.led_mode = LedModeOn; else
		values.led_mode = LedModeOff;
	values.led = xorg_config->flag_bit;

	XChangeKeyboardControl(xorg_config->display, KBLed | KBLedMode, &values);

	// get the state again to apply changes
	XGetKeyboardControl(xorg_config->display, xorg_config->keyboard_state);
}
