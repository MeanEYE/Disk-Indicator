/**
 * Support for notifications using ThinkPad LEDs.
 */

#include <stdlib.h>
#include "thinkpad.h"

/**
 * Initialize ThinkPad notification support.
 */
char thinkpad_init(int argc, const char *argv[])
{
	char result = 0;

	thinkpad_config = malloc(sizeof(ThinkPadConfig));

	return result;
}

/**
 * Free memory taken by ThinkPad notification support config.
 */
void thinkpad_quit()
{
	free(thinkpad_config);
}

/**
 * Turn ThinkPad light on.
 */
char thinkpad_turn_on()
{
	char result = 0;

	return result;
}

/**
 * Turn ThinkPad light off.
 */
char thinkpad_turn_off()
{
	char result = 0;

	return result;
}
