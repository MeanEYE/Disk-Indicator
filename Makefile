PROJECT=disk_indicator
COMPILER=c99
LINK_FLAGS=-lm -lX11
COMPILE_FLAGS=-Wall -pedantic -Werror -D _DEFAULT_SOURCE -O2
OBJECTS=xorg.o console.o thinkpad.o main.o


all: $(PROJECT)

debug: COMPILE_FLAGS += -g
debug: $(PROJECT)

%.o: src/%.c src/%.h
	$(COMPILER) $(COMPILE_FLAGS) -c $<

$(PROJECT): $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINK_FLAGS) -o $(PROJECT)

clean:
	$(RM) $(PROJECT) *.o
