#include "shared.h"

#ifndef THINKPAD_INCLUDED
#define THINKPAD_INCLUDED

#define THINKPAD_ACPI_DEVICE "/proc/acpi/ibm/led"

typedef struct {
	char *led;
	int device;
} ThinkpadConfig;


void thinkpad_init(Indicator *indicator, char *led);
void thinkpad_quit(Indicator *indicator);
void thinkpad_turn_on(Indicator *indicator);
void thinkpad_turn_off(Indicator *indicator);

#endif
