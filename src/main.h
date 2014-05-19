#include <stdbool.h>

#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

enum notify_method {
	X_ORG,
	CONSOLE,
	THINKPAD
};

typedef struct {
	bool initialized;
	enum notify_method method;

	char *old_data, *new_data;

	// pointer to functions used for notifying user
	char (*turn_notification_on)(void);
	char (*turn_notification_off)(void);

	// timeouts measured in CPU cycles
	int check_interval;
	int power_on_interval;
	int power_off_interval;
} Config;

static Config *config;
const unsigned int BUFFER_SIZE = 4096;

Config *load_config(int argc, const char *argv[]);
void unload_config(Config *config);
char set_keyboard_led(Config *config, unsigned char new_state);
char get_led_state(Config *config, unsigned char *state);
void get_disk_stats(void *data, unsigned int size);

#endif
