/**
 * Disk Indicator 0.1
 * Copyright © 2014 by Mladen Mijatov <meaneye.rcf@gmail.com>. 
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
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <linux/kd.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>

#include "main.h"

/**
 * Create new configuration structure and populate
 * with default values if no arguments were passed.
 */
Config *load_config(int argc, const char *argv[]) {
	Config *result = malloc(sizeof(Config));

	// default configuration
	flags.detect_tty = false;
	flags.use_xorg = false;
        int led = LED_SCROLL_LOCK;

	// parse arguments
	while (argc > 1) {
		if (strcmp(argv[1], "-t") == 0) {
			flags.detect_tty = true;

		} else if (strcmp(argv[1], "-x") == 0) {
			flags.use_xorg = true;
		}

                if (strncmp(argv[1], "cap", 3) == 0) {
                        led = LED_CAPS_LOCK;

                } else if (strncmp(argv[1], "num", 3) == 0) {
                        led = LED_NUM_LOCK;
                }

		// shift parameters
		argc--;
		argv++;
	}

	// open console device
	if (flags.detect_tty)
		result->device = open(ttyname(0), O_RDONLY); else
		result->device = open("/dev/console", O_RDONLY);

	result->led = led;
	result->check_interval = 200000;
	result->power_on_interval = 40000;
	result->power_off_interval= 10000;

	// allocate memory for comparison
	result->old_data = (char *) calloc(1, BUFFER_SIZE);
	result->new_data = (char *) calloc(1, BUFFER_SIZE);

	// get X.Org display
	if (flags.use_xorg) {
		result->display = XOpenDisplay(":0");
		result->keyboard_state = malloc(sizeof(XKeyboardState));
	}

	return result;
}

/**
 * Free memory taken by the config.
 */
void unload_config(Config *config) {
	close(config->device);

	// free memory taken by the buffer
	free(config->old_data);
	free(config->new_data);

	// close X.Org display
	if (flags.use_xorg) {
		XCloseDisplay(config->display);
		free(config->keyboard_state);
	}

	free(config);
}

/**
 * Set led on keyboard to specified state.
 */
char set_keyboard_led(Config *config, unsigned char new_state) {
	char result = 0;
	unsigned char current_state = 0;

	// exit if we can't get current state
	if (get_led_state(config, &current_state) == -1) {
		printf("Unable to get current LED state. Are you root?\n");
		exit(1);
	}

	// change state of LEDs
	if (new_state == 1)
		current_state |= config->led; else
		current_state &= ~config->led;

	// apply new state
	if (ioctl(config->device, KDSETLED, current_state))
		result = -1;

	return result;
}

/**
 * Get current state of LEDs.
 */
char get_led_state(Config *config, unsigned char *state) {
	char result = 0;
	unsigned long x_led_mask;

	if (flags.use_xorg) {
		// use Xlib to get key mask
		XGetKeyboardControl(config->display, config->keyboard_state);
		x_led_mask = config->keyboard_state->led_mask;

		// reset state
		*state = 0;

		// form new state
		if ((x_led_mask & X_MASK_CAPS_LOCK) == X_MASK_CAPS_LOCK)
			*state |= LED_CAPS_LOCK;

		if ((x_led_mask & X_MASK_NUM_LOCK) == X_MASK_NUM_LOCK)
			*state |= LED_NUM_LOCK;

		if ((x_led_mask & X_MASK_SCROLL_LOCK) == X_MASK_SCROLL_LOCK)
			*state |= LED_SCROLL_LOCK;

	} else {
		// old fashioned way through ioctl
		if (ioctl(config->device, KDGETLED, state))
			result = -1;
	}

	return result;
}

/**
 * Get content for disk statistics file.
 */
void get_disk_stats(void *data, unsigned int size) {
	FILE *handle;

	// open file
	handle = fopen("/proc/diskstats", "r");
	fread(data, 1, size, handle);

	// close file
	fclose(handle);
}

/**
 * Handle external signals.
 */
void handle_signal(int number) {
	unload_config(config);
	exit(EXIT_SUCCESS);
}

int main(int argc, const char *argv[]) {

        if ((strcmp(argv[1], "-h")==0) | (strcmp(argv[1], "--help")==0)) {
                printf("Usage: disk_indicator [flag] [target]\n\tFlags: -x (X11), -t (TTY)\n\tTargets: scroll (default), num, caps\n");
                exit(EXIT_SUCCESS);
        }

	// register signal handler
	signal(SIGINT, handle_signal);
	
	// load config
	config = load_config(argc, argv);

	while (1) {
		get_disk_stats(config->new_data, BUFFER_SIZE);

		while (memcmp(config->old_data, config->new_data, BUFFER_SIZE) != 0) {
			memcpy(config->old_data, config->new_data, BUFFER_SIZE);
			get_disk_stats(config->new_data, BUFFER_SIZE);

			// turn the light on
			set_keyboard_led(config, 1);
			usleep(config->power_off_interval);

			// turn it off
			set_keyboard_led(config, 0);
			usleep(config->power_on_interval);
		}

		usleep(config->check_interval);
	}

	unload_config(config);
	return EXIT_SUCCESS;
}
