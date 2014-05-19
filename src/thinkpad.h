#ifndef THINKPAD_INCLUDED
#define THINKPAD_INCLUDED

char *thinkpad_led;
FILE *thinkpad_device;

char thinkpad_init(int argc, const char *argv[]);
void thinkpad_quit();
char thinkpad_turn_on();
char thinkpad_turn_off();

#endif
