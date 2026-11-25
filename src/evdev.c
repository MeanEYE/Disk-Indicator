/**
 * Disk Indicator 0.3
 * Copyright © 2026. by Mladen Mijatov <meaneye.rcf@gmail.com>.
 *
 * Support for notification using keyboard LEDs through kernel
 * input event interface. This method doesn't depend on display
 * server and is the one to use on Wayland.
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
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "evdev.h"

#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define LED_MASK_SIZE (LED_CNT / BITS_PER_LONG + 1)


/**
 * Check if bit for specified LED is set in mask returned by kernel.
 */
static bool evdev_test_bit(unsigned long *mask, unsigned short led)
{
	return ((mask[led / BITS_PER_LONG] >> (led % BITS_PER_LONG)) & 1) != 0;
}

/**
 * Check if input device has specified LED.
 */
static bool evdev_has_led(int device, unsigned short led)
{
	unsigned long mask[LED_MASK_SIZE] = {0};

	if (ioctl(device, EVIOCGBIT(EV_LED, sizeof(mask)), mask) < 0)
		return false;

	return evdev_test_bit(mask, led);
}

/**
 * Get current state of the LED on specified input device.
 */
static bool evdev_get_state(int device, unsigned short led)
{
	unsigned long mask[LED_MASK_SIZE] = {0};

	if (ioctl(device, EVIOCGLED(sizeof(mask)), mask) < 0)
		return false;

	return evdev_test_bit(mask, led);
}

/**
 * Set state of the LED on specified input device.
 */
static void evdev_set_state(int device, unsigned short led, bool state)
{
	struct input_event events[2];

	memset(events, 0, sizeof(events));
	events[0].type = EV_LED;
	events[0].code = led;
	events[0].value = state ? 1 : 0;
	events[1].type = EV_SYN;
	events[1].code = SYN_REPORT;

	// device can go away at any point, there's nothing to be done about failure
	if (write(device, events, sizeof(events)) < 0)
		return;
}

/**
 * Open input device and store it if it has LED we are looking for. Errors
 * are reported only for devices explicitly specified by the user.
 */
static void evdev_add_device(EvdevConfig *evdev_config, const char *path, bool report)
{
	if (evdev_config->device_count == EVDEV_MAX_DEVICES)
		return;

	// write only access, we have no interest in reading key presses
	int device = open(path, O_WRONLY);

	if (device < 0) {
		if (report)
			perror(path);
		return;
	}

	if (!evdev_has_led(device, evdev_config->led)) {
		if (report)
			fprintf(stderr, "%s: Device doesn't have requested LED.\n", path);
		close(device);
		return;
	}

	evdev_config->devices[evdev_config->device_count] = device;
	evdev_config->device_count++;
}

/**
 * Initialize keyboard LED notification through input event interface. Config is
 * either LED name alone, in which case all input devices with that LED are used,
 * or input device and LED name separated by pipe character.
 */
void evdev_init(Indicator *indicator, char *config)
{
	char path[512];

	// allocate memory for configuration structure
	EvdevConfig *evdev_config = calloc(1, sizeof(EvdevConfig));
	indicator->config = (char *) evdev_config;
	indicator->initialized = false;

	// split device from led name
	char *device = NULL;
	char *led = config;
	char *separator = strrchr(config, '|');

	if (separator != NULL) {
		*separator = '\0';
		device = config;
		led = separator + 1;
	}

	// get led id
	if (strcmp(led, "scroll") == 0) {
		evdev_config->led = LED_SCROLLL;
	} else if (strcmp(led, "num") == 0) {
		evdev_config->led = LED_NUML;
	} else if (strcmp(led, "caps") == 0) {
		evdev_config->led = LED_CAPSL;
	} else {
		fprintf(stderr, "Unknown LED name: %s\n", led);
		return;
	}

	if (device != NULL) {
		// open specified device, either full path or name in input directory
		if (device[0] == '/')
			snprintf(path, sizeof(path), "%s", device); else
			snprintf(path, sizeof(path), "%s/%s", EVDEV_DEVICE_PATH, device);

		evdev_add_device(evdev_config, path, true);

	} else {
		// open all the devices which have the LED
		DIR *directory = opendir(EVDEV_DEVICE_PATH);
		struct dirent *entry;

		if (directory == NULL) {
			perror(EVDEV_DEVICE_PATH);
			return;
		}

		while ((entry = readdir(directory)) != NULL) {
			if (strncmp(entry->d_name, EVDEV_DEVICE_PREFIX, strlen(EVDEV_DEVICE_PREFIX)) != 0)
				continue;

			snprintf(path, sizeof(path), "%s/%s", EVDEV_DEVICE_PATH, entry->d_name);
			evdev_add_device(evdev_config, path, false);
		}

		closedir(directory);

		if (evdev_config->device_count == 0)
			fprintf(
				stderr,
				"No usable input device with requested LED found in %s, "
				"root or input group access is required.\n",
				EVDEV_DEVICE_PATH
				);
	}

	indicator->initialized = evdev_config->device_count > 0;
}

/**
 * Clean up after us.
 */
void evdev_quit(Indicator *indicator)
{
	EvdevConfig *evdev_config = (EvdevConfig *) indicator->config;

	for (int i = 0; i < evdev_config->device_count; i++)
		close(evdev_config->devices[i]);

	free(evdev_config);
}

/**
 * Turn keyboard LED on. If LED is already lit, for example caps
 * lock is active, it's turned off instead to keep blink visible.
 */
void evdev_turn_on(Indicator *indicator)
{
	EvdevConfig *evdev_config = (EvdevConfig *) indicator->config;

	for (int i = 0; i < evdev_config->device_count; i++) {
		int device = evdev_config->devices[i];

		// store initial state so it can be restored
		evdev_config->initial_state[i] = evdev_get_state(device, evdev_config->led);
		evdev_set_state(device, evdev_config->led, !evdev_config->initial_state[i]);
	}
}

/**
 * Turn keyboard LED off by restoring state it was in.
 */
void evdev_turn_off(Indicator *indicator)
{
	EvdevConfig *evdev_config = (EvdevConfig *) indicator->config;

	for (int i = 0; i < evdev_config->device_count; i++)
		evdev_set_state(
			evdev_config->devices[i],
			evdev_config->led,
			evdev_config->initial_state[i]
			);
}
