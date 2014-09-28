/**
 * Disk Indicator 0.1
 * Copyright © 2014 by Mladen Mijatov <meaneye.rcf@gmail.com>.
 *
 * Support for notifications using ThinkPad LEDs.
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
#include "thinkpad.h"

/**
 * Initialize ThinkPad notification support.
 */
char thinkpad_init(int argc, const char *argv[])
{
	char result = 0;

	// default config
	thinkpad_led = "0";

	// parse parameters
	if (argc > 0)
		thinkpad_led = (char*) strdup(argv[0]);

	// open acpi device
	thinkpad_device = fopen("/proc/acpi/ibm/led", "w");

	if (!thinkpad_device)
		result = -1;

	return result;
}

/**
 * Free memory taken by ThinkPad notification support config.
 */
void thinkpad_quit()
{
	thinkpad_turn_on();
	fclose(thinkpad_device);
}

/**
 * Turn ThinkPad light on.
 */
char thinkpad_turn_on()
{
	char result = 0;

	// set new state
	fputs(thinkpad_led, thinkpad_device);
	fputs(" on\n", thinkpad_device);

	// flush to apply
	fflush(thinkpad_device);

	return result;
}

/**
 * Turn ThinkPad light off.
 */
char thinkpad_turn_off()
{
	char result = 0;

	// set new state
	fputs(thinkpad_led, thinkpad_device);
	fputs(" off\n", thinkpad_device);

	// flush to apply
	fflush(thinkpad_device);

	return result;
}
