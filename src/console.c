/**
 * Disk Indicator 0.1
 * Copyright © 2014 by Mladen Mijatov <meaneye.rcf@gmail.com>.
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
char console_init(int argc, const char *argv[])
{
	char result = 0;

	// allocate memory for configuration structure
	console_config = malloc(sizeof(ConsoleConfig));

	// default parameters
	console_config->led = SCROLL_LOCK;
	console_config->tty[0] = '\0';
	strcat(console_config->tty, "/dev/");

	// parse parameters
	if (argc > 0)
		strcat(console_config->tty, argv[0]); else
		strcat(console_config->tty, "console");

	if (argc == 2) {
		if (strncmp(argv[1], "scr", 3) == 0) {
			console_config->led = SCROLL_LOCK;
		} else if (strncmp(argv[1], "num", 3) == 0) {
			console_config->led = NUM_LOCK;
		} else if (strncmp(argv[1], "cap", 3) == 0) {
			console_config->led = CAPS_LOCK;
		}
	}

	// open console interface
	console_config->device = open(console_config->tty, O_RDWR);

	if (console_config->device == -1)
		result = -1;

	return result;
}

/**
 * Clean up after us.
 */
void console_quit()
{
	close(console_config->device);
	free(console_config);
}

/**
 * Turn keyboard LED on.
 */
char console_turn_on()
{
	char result = 0;
	unsigned char state = 0;

	// get current state
	ioctl(console_config->device, KDGETLED, &state);

	// toggle state of specified LED
	state |= console_config->led;

	// apply new state
	if (ioctl(console_config->device, KDSETLED, state))
		result = -1;

	return result;
}

/**
 * Turn keyboard LED off.
 */
char console_turn_off()
{
	char result = 0;
	unsigned char state;

	// get current state
	ioctl(console_config->device, KDGETLED, &state);

	// toggle state of specified LED
	state &= ~console_config->led;

	// apply new state
	if (ioctl(console_config->device, KDSETLED, state))
		result = -1;

	return result;
}
