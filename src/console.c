/**
 * Disk Indicator 0.2
 * Copyright © 2015. by Mladen Mijatov <meaneye.rcf@gmail.com>.
 *
 * Support for notification using keyboard LEDs through
 * standard TTY interface.
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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include "console.h"

/**
 * Initialize keyboard LED notification through TTY.
 */
void console_init(Indicator *indicator, char *config)
{
	// allocate memory for configuration structure
	ConsoleConfig *console_config = malloc(sizeof(ConsoleConfig));
	indicator->config = (char *) console_config;

	// parse config
	char device[10] = {0};
	char led[10] = {0};

	// get tty device
	console_config->tty[0] = '\0';
	strcat(console_config->tty, "/dev/");
	if (sscanf(config, XORG_CONFIG_FORMAT, device, led) == 2)
		strcat(console_config->tty, device); else
		strcat(console_config->tty, "console");

	// get led id
	if (strcmp(led, "scroll") == 0) {
		console_config->led = SCROLL_LOCK;
	} else if (strcmp(led, "num") == 0) {
		console_config->led = NUM_LOCK;
	} else {
		console_config->led = CAPS_LOCK;
	}

	// open console interface
	console_config->device = open(console_config->tty, O_RDWR);

	if (console_config->device == -1)
		indicator->initialized = false; else
		indicator->initialized = true;
}

/**
 * Clean up after us.
 */
void console_quit(Indicator *indicator)
{
	ConsoleConfig *console_config = (ConsoleConfig *) indicator->config;

	if (indicator->initialized)
		close(console_config->device);
	free(console_config);
}

/**
 * Turn keyboard LED on.
 */
void console_turn_on(Indicator *indicator)
{
	unsigned char state;
	ConsoleConfig *console_config = (ConsoleConfig *) indicator->config;

	// get current state
	ioctl(console_config->device, KDGETLED, &state);

	// toggle state of specified LED
	state |= console_config->led;

	// apply new state
	ioctl(console_config->device, KDSETLED, state);
}

/**
 * Turn keyboard LED off.
 */
void console_turn_off(Indicator *indicator)
{
	unsigned char state;
	ConsoleConfig *console_config = (ConsoleConfig *) indicator->config;

	// get current state
	ioctl(console_config->device, KDGETLED, &state);

	// toggle state of specified LED
	state &= ~console_config->led;

	// apply new state
	ioctl(console_config->device, KDSETLED, state);
}
