#include <stdbool.h>
#include "shared.h"

#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#define FILENAME "/proc/diskstats"
#define STATS_FORMAT "%*d %*d %s %lu %*u %*u %*u %lu %*u %*u %*u %*u %*u %*u"
#define CONFIG_FORMAT "led=%s event=%s device=%s"


typedef struct {
	bool initialized;

	// statistics file handle
	int stats_file;

	// configured indicators
	Indicator *indicators[20];
	int indicator_count;

	// timeouts measured in CPU cycles
	int check_interval;
	int power_on_interval;
	int power_off_interval;

	// method flags
	bool xorg_initialized;
	bool console_initialized;
	bool thinkpad_initialized;
} Config;


Config *config;

Config *load_config(int argc, const char *argv[]);
void unload_config(Config *config);
unsigned int read_line(int file, char *line);
bool open_stats_file();
void close_stats_file();
char set_keyboard_led(Config *config, unsigned char new_state);
char get_led_state(Config *config, unsigned char *state);
void get_disk_stats(void *data, unsigned int size);

#endif
