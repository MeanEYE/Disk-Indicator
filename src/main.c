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

#define _POSIX_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include "main.h"
#include "xorg.h"
#include "console.h"
#include "thinkpad.h"

/**
 * Create new configuration structure and populate
 * with default values if no arguments were passed.
 */
Config *load_config(int argc, const char *argv[])
{
	Config *result = malloc(sizeof(Config));
	char init_result = 0;

	// default configuration
	result->initialized = false;
	result->method = X_ORG;
	result->check_interval = 200000;
	result->power_on_interval = 40000;
	result->power_off_interval= 10000;

	// allocate memory for comparison
	result->old_data = (char *) calloc(1, BUFFER_SIZE);
	result->new_data = (char *) calloc(1, BUFFER_SIZE);

	// shift parameters as first param is just program executable
	argc--;
	argv++;

	// parse arguments
	if (argc > 0) {
		if (strncmp(argv[0], "-x", 2) == 0) {
			result->method = X_ORG;

		} else if (strncmp(argv[0], "-c", 2) == 0) {
			result->method = CONSOLE;

		} else if (strncmp(argv[0], "-t", 2) == 0) {
			result->method = THINKPAD;
		}

	} else {
		// default method
		result->method = X_ORG;
	}

	// shift parameters
	argc--;
	argv++;

	// initialize specified subsystem
	switch (result->method) {
		case X_ORG:
			init_result = xorg_init(argc, argv);
			result->turn_notification_on = &xorg_turn_on;
			result->turn_notification_off = &xorg_turn_off;
			break;

		case CONSOLE:
			init_result = console_init(argc, argv);
			result->turn_notification_on = &console_turn_on;
			result->turn_notification_off = &console_turn_off;
			break;

		case THINKPAD:
			init_result = thinkpad_init(argc, argv);
			result->turn_notification_on = &thinkpad_turn_on;
			result->turn_notification_off = &thinkpad_turn_off;
			break;
	}

	// mark subsystem as initialized
	if (init_result == 0)
		result->initialized = true;

	return result;
}

/**
 * Free memory taken by the config.
 */
void unload_config(Config *config)
{
	// close disk statistics file
	close_stats_file();

	// free memory taken by the buffer
	free(config->old_data);
	free(config->new_data);

	// let subsystem clean up after themselves
	if (config->initialized)
		switch (config->method) {
			case X_ORG:
				xorg_quit();
				break;

			case CONSOLE:
				console_quit();
				break;

			case THINKPAD:
				thinkpad_quit();
				break;
		}

	free(config);
}

/**
 * Open disk statistics file.
 */
bool open_stats_file()
{
	bool result = true;

	// open file
	config->stats_file = open(FILENAME, O_RDONLY);
	if (config->stats_file < 0) {
		perror(FILENAME);
		result = false;
	}

	return result;
}

/**
 * Close disk statistics file.
 */
void close_stats_file()
{
	close(config->stats_file);
}

/**
 * Get content for disk statistics file.
 */
void get_disk_stats(void *data, unsigned int size)
{
	lseek(config->stats_file, 0, SEEK_SET);
	read(config->stats_file, data, size);
}

/**
 * Handle external signals.
 */
void handle_signal(int number)
{
	unload_config(config);
	exit(EXIT_SUCCESS);
}

/**
 * Check if we're attached to a terminal.
 */
int is_attached(void)
{
	return isatty(
		fileno(stdout) &&
		isatty(fileno(stdin)) &&
		isatty(fileno(stderr))
		);
}

/**
 * Daemonize to background and detach from console.
 */
void daemonize(void)
{
	if (is_attached()) {
		int i = fork();

		// exit our parent process
		if (i > 0)
			exit(EXIT_SUCCESS);

		// say our method and PID to the console
		switch (config->method) {
			case X_ORG:
				printf("Using X.Org method.");
				break;

			case CONSOLE:
				printf("Using TTY method.");
				break;

			case THINKPAD:
				printf("Using Thinkpad LED method.");
				break;
		}

		printf(" Going away from console, pid: %d\n", getpid());

		// close file descriptors
		freopen("/dev/null", "r", stdin);
		freopen("/dev/null", "w", stdout);
		freopen("/dev/null", "w", stderr);

		if (setpgid(0, 0) < 0)
			exit(EXIT_FAILURE);
		else if (i == -1)
			exit(EXIT_FAILURE);
	}
}

/**
 * Main application loop.
 */
int main(int argc, const char *argv[])
{
	int no_fork = 0;

	// show help if no arguments are specified
	if (argc > 1 && ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))) {
		printf(
			"Usage: disk_indicator <method> [parameters]\n"
			"Methods:\n"
			"\t-x\tUse X.Org server to set keyboard LEDs.\n"
			"\t-c\tUse TTY console interfact to set keyboard LEDs.\n"
			"\t-t\tUse ThinkPad LEDs for notification.\n"
			"\t-f\tDo not fork to background.\n\n"
			"X.Org parameters: [led]\n"
			"\tnum\tUse NumLock\n"
			"\tcap\tUse CapsLock\n"
			"\tscr\tUse ScrollLock (default)\n\n"
			"TTY console parameters: [device [led]]\n"
			"\ttty[1-9]\n"
			"\tconsole\t(default)\n\n"
			"\tnum\tUse NumLock\n"
			"\tcap\tUse CapsLock\n"
			"\tscr\tUse ScrollLock (default)\n\n"
			"ThinkPad parameters: [led]\n"
			"\t0-15\n"
			);
		exit(EXIT_SUCCESS);

	}

	if (argc > 1 && strcmp(argv[1], "-f") == 0) {
		// set forking flag
		no_fork = 1;

		// shift parameters
		argc--;
		argv++;
	}

	// register signal handler
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	// load config
	config = load_config(argc, argv);

	// check if config was successfully initialized
	if (!config->initialized) {
		unload_config(config);
		printf("Error initializing specified subsystem.\n");
		exit(1);
	}

	// try opening disk statistics
	if (open_stats_file() == false) {
		unload_config(config);
		exit(2);
	}

	// fork to background
	if (!no_fork)
		daemonize();

	// start main loop
	while (1) {
		get_disk_stats(config->new_data, BUFFER_SIZE);

		while (memcmp(config->old_data, config->new_data, BUFFER_SIZE) != 0) {
			memcpy(config->old_data, config->new_data, BUFFER_SIZE);
			get_disk_stats(config->new_data, BUFFER_SIZE);

			// turn the light on
			config->turn_notification_on();
			usleep(config->power_on_interval);

			// turn it off
			config->turn_notification_off();
			usleep(config->power_off_interval);
		}

		usleep(config->check_interval);
	}

	unload_config(config);
	return EXIT_SUCCESS;
}
