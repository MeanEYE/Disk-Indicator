#include <linux/kd.h>

#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#define LED_SCROLL_LOCK LED_SCR
#define LED_NUM_LOCK LED_NUM
#define LED_CAPS_LOCK LED_CAP

typedef struct {
	int device;
	char *old_data, *new_data;

	char led;
	int check_interval;
	int power_on_interval;
	int power_off_interval;
} Config;

static Config *config;
const unsigned int BUFFER_SIZE = 4096;

Config *load_config(int argc, const char *argv[]);
void unload_config(Config *config);
char set_keyboard_led(Config *config, char new_state);
char get_led_state(Config *config, char *state);
void get_disk_stats(void *data, unsigned int size);

#endif
