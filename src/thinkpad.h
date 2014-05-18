#ifndef THINKPAD_INCLUDED
#define THINKPAD_INCLUDED

typedef struct {
	char led[15];
} ThinkPadConfig;

static ThinkPadConfig *thinkpad_config;

char thinkpad_init(int argc, const char *argv[]);
void thinkpad_quit();
char thinkpad_turn_on();
char thinkpad_turn_off();

#endif
