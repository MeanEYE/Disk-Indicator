PROJECT=disk_indicator
COMPILER=c99
LINK_FLAGS=-lm -lX11
COMPILE_FLAGS=-Wall -pedantic -Werror -g -D _BSD_SOURCE
OBJECTS=xorg.o console.o thinkpad.o main.o

all: $(PROJECT)

%.o: src/%.c src/%.h
	$(COMPILER) $(COMPILE_FLAGS) -c -s $<

$(PROJECT): $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(LINK_FLAGS) -o $(PROJECT)

clean:
	rm $(PROJECT) *.o
