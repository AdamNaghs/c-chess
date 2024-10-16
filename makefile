# Variables
CC := gcc
RM := rm -f
CFLAGS := -Wall -Werror -g -std=c99 #-fsanitize=address
EXE := a
SRC_DIR := src
OBJ_DIR := obj
CFILES := $(wildcard $(SRC_DIR)/*.c)
OFILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CFILES))

# Default target
all: build

# Target to build the executable
build: $(OFILES)
	$(CC) $(CFLAGS) -o $(EXE) $(OFILES)

# Rule to compile .c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up build files
clean:
	$(RM) $(OFILES) $(EXE)
	$(RM) $(EXE).*
	$(RM) -r $(OBJ_DIR)

# Run the program
run: build
	./$(EXE)

.PHONY: all build clean run