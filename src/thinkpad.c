/**
 * Disk Indicator 0.2
 * Copyright © 2015. by Mladen Mijatov <meaneye.rcf@gmail.com>.
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
#include <fcntl.h>
#include <unistd.h>
#include "thinkpad.h"
#include "shared.h"


/**
 * Initialize ThinkPad notification support.
 */
void thinkpad_init(Indicator *indicator, char *led)
{
	// allocate memory for configuration structure
	ThinkpadConfig *thinkpad_config = calloc(1, sizeof(ThinkpadConfig));
	indicator->config = (char *) thinkpad_config;

	// default config
	thinkpad_config->led = calloc(1, strlen(led) + 1);
	strcpy(thinkpad_config->led, led);

	// open acpi device
	thinkpad_config->device = open(THINKPAD_ACPI_DEVICE, O_RDONLY);

	if (!thinkpad_config->device)
		indicator->initialized = false; else
		indicator->initialized = true;
}

/**
 * Free memory taken by ThinkPad notification support config.
 */
void thinkpad_quit(Indicator *indicator)
{
	ThinkpadConfig *thinkpad_config = (ThinkpadConfig *) indicator->config;

	// free device
	if (indicator->initialized) {
		indicator->turn_notification_on(indicator);
		close(thinkpad_config->device);
	}

	// free memory
	free(thinkpad_config->led);
	free(thinkpad_config);
}

/**
 * Turn ThinkPad light on.
 */
void thinkpad_turn_on(Indicator *indicator)
{
	ThinkpadConfig *thinkpad_config = (ThinkpadConfig *) indicator->config;

	// set new state
	write(thinkpad_config->device, thinkpad_config->led, strlen(thinkpad_config->led));
	write(thinkpad_config->device, " on\n", 4);
	fsync(thinkpad_config->device);
}

/**
 * Turn ThinkPad light off.
 */
void thinkpad_turn_off(Indicator *indicator)
{
	ThinkpadConfig *thinkpad_config = (ThinkpadConfig *) indicator->config;

	// set new state
	write(thinkpad_config->device, thinkpad_config->led, strlen(thinkpad_config->led));
	write(thinkpad_config->device, " off\n", 5);
	fsync(thinkpad_config->device);
}
