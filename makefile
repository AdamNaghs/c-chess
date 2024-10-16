# Variables
cc := gcc
rm := rm
cflags := -Wall -Werror -g -std=c99 #-fsanitize=address
exe := a
cfiles := $(wildcard *.c)
ofiles := $(patsubst %.c,%.o,$(cfiles))

# Target to build the executable
build: $(ofiles)
	$(cc) $(cflags) -o $(exe).exe $(ofiles)

# Rule to compile .c files into .o files
%.o: %.c
	$(cc) $(cflags) -c $< -o $@

# Clean up build files
clean:
	$(rm) $(ofiles) $(exe)
	$(rm) $(exe).*

run:
	./$(exe)

.PHONY: build clean
