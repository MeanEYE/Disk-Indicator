PROJECT=disk_indicator
COMPILER=c99
LINK_FLAGS=-lm $(shell cat build/flags)
COMPILE_FLAGS=-Wall -Wextra -pedantic -Werror -D _DEFAULT_SOURCE -O2 -march=native
OBJECTS=$(shell cat build/objects) main.o

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

clean:
	$(RM) -r build/
	$(RM) $(PROJECT) *.o
