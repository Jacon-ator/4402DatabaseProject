# Compiler and flags
CC = g++
CFLAGS = -Wall -g
LDFLAGS = -Llib -lsqlite3

# Project files
TARGET = main
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:.c=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	if exist src\main.o del /f /q src\main.o
	if exist main del /f /q main

# Run the executable
run: all
	./$(TARGET)
