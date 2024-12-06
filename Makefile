# Compiler and flags
CC = g++
CFLAGS = -Wall -g
LDFLAGS = -lsqlite3

# Project files
TARGET = main
SRC = main.c
OBJ = $(SRC:.c=.o)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the executable
run: all
	./$(TARGET)
