# Compiler
CC = gcc
CFLAGS = -g -Wall -I$(GAMEOBJECTS_DIR) -I$(UTIL_DIR)

# Directories
UTIL_DIR = util
GAMEOBJECTS_DIR = gameObjects

# Source files
SRC = gameScreen.c \
      $(UTIL_DIR)/point.c $(UTIL_DIR)/rectangle.c \
      $(GAMEOBJECTS_DIR)/dungeon.c $(GAMEOBJECTS_DIR)/tile.c

# Object files
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = mapMaker

# Build target
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET)
