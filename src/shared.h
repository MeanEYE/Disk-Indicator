#include <stdbool.h>

#ifndef SHARED_INCLUDED
#define SHARED_INCLUDED

enum notify_method {
	X_ORG,
	CONSOLE,
	THINKPAD
};

enum indicator_event {
	READ,
	WRITE,
	BOTH
};

typedef struct Indicator {
	bool initialized;
	enum notify_method method;
	enum indicator_event event;
	char *device;

	// pointer to functions used for notifying user
	void (*turn_notification_on)(struct Indicator *indicator);
	void (*turn_notification_off)(struct Indicator *indicator);

	// pointer to method config
	char *config;

	// history
	unsigned long read_count;
	unsigned long write_count;
} Indicator;

#endif
