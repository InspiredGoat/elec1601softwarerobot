SOURCES = $(wildcard *.c)
OBJ = $(addsuffix .o, $(addprefix bin/, $(basename $(SOURCES))));
INCLUDE = -I include
CFLAGS = -W -O2

all: app

again: clean app

app: $(SOURCES)
	gcc -W $^ -lSDL2 -lm -ldl -o $@

clean:
	rm -f bin/*
	rm -f app

try: clean app
	./app

run:
	./app
