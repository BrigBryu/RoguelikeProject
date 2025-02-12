CC = gcc
CFLAGS = -g -Wall -I$(GAMEOBJECTS_DIR) -I$(UTIL_DIR)

#file stuff
UTIL_DIR = util
GAMEOBJECTS_DIR = gameObjects
SRC = gameScreen.c \
      $(UTIL_DIR)/point.c $(UTIL_DIR)/rectangle.c \
      $(GAMEOBJECTS_DIR)/dungeon.c $(GAMEOBJECTS_DIR)/tile.c

# Other
OBJ = $(SRC:.c=.o)
TARGET = assignment01

# first rule
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
