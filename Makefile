PROJECT=disk_indicator
COMPILER=c99
LINK_FLAGS=-lm $(shell cat build/flags)
COMPILE_FLAGS=-Wall -Wextra -pedantic -Werror -D _DEFAULT_SOURCE -O2 -march=native
OBJECTS=$(shell cat build/objects) main.o

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
SYSCONFDIR ?= /etc
UNITDIR ?= $(PREFIX)/lib/systemd/system
CONFIG_FILE=$(SYSCONFDIR)/disk-indicator.conf
UNIT_FILE=$(UNITDIR)/disk-indicator.service

.PHONY: all debug install uninstall clean

all: $(PROJECT)

debug: COMPILE_FLAGS += -g
debug: $(PROJECT)

.check-dependencies:
ifeq ("$(realpath ./build/dependencies.h)","")
	$(error Missing dependencies file. To generate one, please run ./configure.sh)
endif

%.o: src/%.c src/%.h
	$(COMPILER) $(COMPILE_FLAGS) -c $<

$(PROJECT): .check-dependencies $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINK_FLAGS) -o $(PROJECT)

# program is not built here on purpose, to avoid compiling as root
install:
	@test -x $(PROJECT) || { echo "Program is not built yet, please run make first."; exit 1; }
	install -D -m 755 $(PROJECT) $(DESTDIR)$(BINDIR)/$(PROJECT)
	install -d $(DESTDIR)$(UNITDIR)
	sed -e 's|@BINDIR@|$(BINDIR)|g' -e 's|@CONFIG_FILE@|$(CONFIG_FILE)|g' \
		systemd/disk-indicator.service.in > $(DESTDIR)$(UNIT_FILE)
	chmod 644 $(DESTDIR)$(UNIT_FILE)
	@# keep existing configuration
	test -e $(DESTDIR)$(CONFIG_FILE) || install -D -m 644 systemd/disk-indicator.conf $(DESTDIR)$(CONFIG_FILE)
	@echo "Installed. Edit $(CONFIG_FILE), then run:"
	@echo "  systemctl daemon-reload"
	@echo "  systemctl enable --now disk-indicator"

# configuration file is left in place
uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(PROJECT) $(DESTDIR)$(UNIT_FILE)

clean:
	$(RM) -r build/
	$(RM) $(PROJECT) *.o
