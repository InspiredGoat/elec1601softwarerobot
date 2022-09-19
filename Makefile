SOURCES = $(wildcard *.c)
OBJ = $(addsuffix .o, $(addprefix bin/, $(basename $(SOURCES))));
INCLUDE = -I include
CFLAGS = -W -O1

all: app

again: clean app

app: $(SOURCES)
	gcc -W $^ -lSDL2 -lm -ldl -o $@

clean:
	rm -f bin/*
	rm app

install:
	echo "Can't install surry"

try: app
	./app

run:
	./app
