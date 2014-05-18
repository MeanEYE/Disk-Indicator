#include <stdbool.h>
#include <linux/kd.h>
#include <X11/Xlib.h>

#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED

#define LED_SCROLL_LOCK LED_SCR
#define LED_NUM_LOCK LED_NUM
#define LED_CAPS_LOCK LED_CAP

#define X_MASK_CAPS_LOCK 0x1
#define X_MASK_NUM_LOCK 0x2
#define X_MASK_SCROLL_LOCK 0x4

typedef struct {
	int device;
	char *old_data, *new_data;

	// X.Org stuff
	Display *display;
	XKeyboardState *keyboard_state;

	char led;
	int check_interval;
	int power_on_interval;
	int power_off_interval;
} Config;

struct Flags {
	bool detect_tty;
	bool use_xorg;
};

static Config *config;
static struct Flags flags;
const unsigned int BUFFER_SIZE = 4096;

Config *load_config(int argc, const char *argv[]);
void unload_config(Config *config);
char set_keyboard_led(Config *config, unsigned char new_state);
char get_led_state(Config *config, unsigned char *state);
void get_disk_stats(void *data, unsigned int size);

#endif
