#pragma once
#define EVDEV_AVAILABLE

#include <stdbool.h>
#include <linux/input.h>
#include "shared.h"

#define EVDEV_DEVICE_PATH "/dev/input"
#define EVDEV_DEVICE_PREFIX "event"
#define EVDEV_MAX_DEVICES 16


typedef struct {
	// opened input devices and LED state found before turning it on
	int devices[EVDEV_MAX_DEVICES];
	bool initial_state[EVDEV_MAX_DEVICES];
	int device_count;

	unsigned short led;
} EvdevConfig;


void evdev_init(Indicator *indicator, char *config);
void evdev_quit(Indicator *indicator);
void evdev_turn_on(Indicator *indicator);
void evdev_turn_off(Indicator *indicator);
