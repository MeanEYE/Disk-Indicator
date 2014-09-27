/**
 * Support for notifications using ThinkPad LEDs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thinkpad.h"

/**
 * Initialize ThinkPad notification support.
 */
char thinkpad_init(int argc, const char *argv[])
{
	char result = 0;

	// default config
	thinkpad_led = "0";

	// parse parameters
	if (argc > 0)
		thinkpad_led = (char*) strdup(argv[0]);

	// open acpi device
	thinkpad_device = fopen("/proc/acpi/ibm/led", "w");

	if (!thinkpad_device)
		result = -1;

	return result;
}

/**
 * Free memory taken by ThinkPad notification support config.
 */
void thinkpad_quit()
{
	thinkpad_turn_on();
	fclose(thinkpad_device);
}

/**
 * Turn ThinkPad light on.
 */
char thinkpad_turn_on()
{
	char result = 0;

	// set new state
	fputs(thinkpad_led, thinkpad_device);
	fputs(" on\n", thinkpad_device);

	// flush to apply
	fflush(thinkpad_device);

	return result;
}

/**
 * Turn ThinkPad light off.
 */
char thinkpad_turn_off()
{
	char result = 0;

	// set new state
	fputs(thinkpad_led, thinkpad_device);
	fputs(" off\n", thinkpad_device);

	// flush to apply
	fflush(thinkpad_device);

	return result;
}
