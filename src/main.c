/**
 * Disk Indicator 0.2
 * Copyright © 2015. by Mladen Mijatov <meaneye.rcf@gmail.com>.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>

#include "main.h"
#include "xorg.h"
#include "console.h"
#include "thinkpad.h"

char stat_device[20];
unsigned long stat_read_count;
unsigned long stat_write_count;

/**
 * Create new configuration structure and populate
 * with default values if no arguments were passed.
 */
Config *load_config(int argc, const char *argv[])
{
	Config *result = calloc(1, sizeof(Config));
	int config;
	char config_file[4096] = {0};
	char default_config_file[] = "~/.disk-indicator";

	// set default values
	result->xorg_initialized = false;
	result->console_initialized = false;
	result->thinkpad_initialized = false;

	// get config file
	if (argc > 0 && strcmp(argv[0], "-c") == 0) {
		argc--;
		argv++;

		strcpy(config_file, argv[0]);

	} else {
		// use default config file
		strcpy(config_file, default_config_file);
	}

	// load config file
	config = open(config_file, O_RDONLY);
	if (config < 0) {
		perror(config_file);
		goto _exit;
	}

	char led[20];
	char device[20];
	char event[6];
	char line[128];

	while (read_line(config, line) > 0) {
		// parse line from config file
		if (sscanf(line, CONFIG_FORMAT, led, event, device) < 3)
			continue;

		// make sure we don't go over our limit
		if (result->indicator_count == 20) {
			printf("Indicator limit (20) is reached, ignoring the rest of the config file.\n");
			break;
		}

		// create new indicator
		Indicator *indicator = calloc(1, sizeof(Indicator));

		// store device name to indicator
		indicator->device = calloc(1, strlen(device) + 1);
		strcpy(indicator->device, device);

		// assign function calls
		switch (led[0]) {
			case 'x':
				indicator->method = X_ORG;
				indicator->turn_notification_on = &xorg_turn_on;
				indicator->turn_notification_off = &xorg_turn_off;

				xorg_init(indicator, led + 2);
				result->xorg_initialized |= indicator->initialized;
				break;

			case 'c':
				indicator->method = CONSOLE;
				indicator->turn_notification_on = &console_turn_on;
				indicator->turn_notification_off = &console_turn_off;

				console_init(indicator, led + 2);
				result->console_initialized |= indicator->initialized;
				break;

			case 't':
				indicator->method = THINKPAD;
				indicator->turn_notification_on = &thinkpad_turn_on;
				indicator->turn_notification_off = &thinkpad_turn_off;

				thinkpad_init(indicator, led + 2);
				result->thinkpad_initialized |= indicator->initialized;
				break;
		}

		// assign event type
		if (strcmp(event, "read") == 0) {
			indicator->event = READ;
		} else if (strcmp(event, "write") == 0) {
			indicator->event = WRITE;
		} else {
			indicator->event = BOTH;
		}

		// reset counters
		indicator->read_count = 0;
		indicator->write_count = 0;

		// store indicator
		result->indicators[result->indicator_count] = indicator;
		result->indicator_count++;
	}

	// close config file
	close(config);

	// default configuration
	result->check_interval = 200000;
	result->power_on_interval = 40000;
	result->power_off_interval= 10000;

_exit:
	// set config initialization result value
	result->initialized = false;
	result->initialized |= result->xorg_initialized;
	result->initialized |= result->console_initialized;
	result->initialized |= result->thinkpad_initialized;

	if (result->initialized)
		printf("Loaded %d indicators.\n", result->indicator_count);

	return result;
}

/**
 * Free memory taken by the config.
 */
void unload_config(Config *config)
{
	// close disk statistics file
	close_stats_file();

	// free indicator memory
	for (int i = 0; i < config->indicator_count; i++) {
		Indicator *indicator = config->indicators[i];

		// allow subsystems to clean up
		switch (indicator->method) {
			case X_ORG:
				xorg_quit(indicator);
				break;

			case CONSOLE:
				console_quit(indicator);
				break;

			case THINKPAD:
				thinkpad_quit(indicator);
				break;
		}

		free(config->indicators[i]->device);
		free(config->indicators[i]);
	}

	free(config);
}

/**
 * Read single line from specified file descriptor.
 */
unsigned int read_line(int file, char *line)
{
	int length = 0, read_count = 0;
	char character[2];

	while (true) {
		read_count = read(file, character, 1);

		// end of file
		if (read_count == 0)
			break;

		// new line
		if (character[0] == '\0' || character[0] == '\n')
			break;

		line[length] = character[0];
		length += read_count;
	}

	line[length] = '\0';

	return length;
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
	if (config->stats_file)
		close(config->stats_file);
}

/**
 * Get content for disk statistics file.
 */
void read_stats(char *device, unsigned long *read_count, unsigned long *write_count)
{
	char buff[200] = {0};
	unsigned int count = read_line(config->stats_file, buff);

	// go to beginning of the file
	if (count == 0) {
		lseek(config->stats_file, 0, SEEK_SET);

		// let check indicators know we reached end of file
		strcpy(device, "null");
		read_count = 0;
		write_count = 0;
		return;
	}

	// parse line
	sscanf(buff, STATS_FORMAT, device, read_count, write_count);
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
	if (!is_attached())
		return;

	// fork process
	int pid = fork();

	// exit our parent process
	if (pid > 0)
		exit(EXIT_SUCCESS);

	printf("Going away from console, pid: %d\n", getpid());

	// close file descriptors
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	if (setpgid(0, 0) < 0 || pid == -1)
		exit(EXIT_FAILURE);
}

/**
 * Check all indicators and return number of currently active ones.
 */
char check_indicators(void)
{
	char result = 0;
	bool changed;

	read_stats(stat_device, &stat_read_count, &stat_write_count);

	while (strcmp(stat_device, "null") != 0) {
		for (int i = 0; i < config->indicator_count; i++) {
			Indicator *indicator = config->indicators[i];

			// jump to next indicator
			if (strcmp(stat_device, indicator->device) != 0)
				continue;

			// compare data
			changed = false;

			if (indicator->event == READ || indicator->event == BOTH)
				changed |= stat_read_count != indicator->read_count;

			if (indicator->event == WRITE || indicator->event == BOTH)
				changed |= stat_write_count != indicator->write_count;

			// store new data
			indicator->read_count = stat_read_count;
			indicator->write_count = stat_write_count;

			// turn the light on
			if (changed) {
				indicator->turn_notification_on(indicator);
				usleep(config->power_on_interval);

				// turn it off
				indicator->turn_notification_off(indicator);
				usleep(config->power_off_interval);

				// increase result
				result++;
			}
		}

		// read stats again
		read_stats(stat_device, &stat_read_count, &stat_write_count);
	}

	return result;
}

/**
 * Main application loop.
 */
int main(int argc, const char *argv[])
{
	int no_fork = 0;

	// shift arguments, first is just program executable
	argc--;
	argv++;

	// show help if no arguments are specified
	if (argc > 0 && ((strcmp(argv[0], "-h") == 0) || (strcmp(argv[0], "--help") == 0))) {
		printf(
			"Usage: disk_indicator [-f] [-c config.file]\n"
			"\t-c\tLoad specified config. (default: ~/.disk-indicator)\n"
			"\t-f\tDo not fork to background.\n\n"
			"Sample config file:\n"
			"\tled=t|0 event=read device=sda\n"
			"\tled=c|tty1|caps event=write device=sda\n"
			"\tled=x|scroll event=both device=sda1\n\n"
			"Config params:\n"
			"\tled=<provider>|<name>\tProvider: t, c, x\tName: 0-15, caps, scroll, num\n"
			"\tevent=<type>\t\tType: read, write, both\n"
			"\tdevice=<name>\t\tName: eg. sda1, sda, mmcblkp1\n\n"
			);
		exit(EXIT_SUCCESS);
	}

	if (argc > 0 && strcmp(argv[0], "-f") == 0) {
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
		printf("Error loading configuration file.\n");
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
		if (check_indicators() == 0)
			usleep(config->check_interval);
	}

	unload_config(config);
	return EXIT_SUCCESS;
}
