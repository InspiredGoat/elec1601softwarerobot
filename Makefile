SOURCES = $(wildcard ./*.c)
SRC = $(addprefix src/, $(SOURCES))
OBJ = $(addsuffix .o, $(addprefix bin/, $(basename $(notdir $(SRC)))));
INCLUDE = -I include
CFLAGS = -W -O1

all: app

again: clean app

app: $(OBJ)
	echo $(SOURCES)
	clang -W $^ -lSDL2 -lm -ldl -o $@

bin/%.o : %.c
	clang $(INCLUDE) $(CFLAGS) -c $< -o $@

clean:
	rm -f bin/*
	rm app

install:
	echo "Can't install surry"

try: app
	./app

run:
	./app
