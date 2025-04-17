CC = g++
CFLAGS = -g -Wall -I$(GAMEOBJECTS_DIR) -I$(UTIL_DIR)

# File directories
UTIL_DIR = util
GAMEOBJECTS_DIR = gameObjects

SRC = gameScreen.cpp \
      $(UTIL_DIR)/point.cpp \
      $(UTIL_DIR)/rectangle.cpp \
      $(UTIL_DIR)/fileHandle.cpp \
      $(UTIL_DIR)/heap.cpp \
      $(UTIL_DIR)/ui.cpp \
      $(UTIL_DIR)/monsterParser.cpp \
      $(GAMEOBJECTS_DIR)/dungeon.cpp \
      $(GAMEOBJECTS_DIR)/tile.cpp \
      $(GAMEOBJECTS_DIR)/simulate.cpp \
      $(GAMEOBJECTS_DIR)/npc.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = assignment07

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lncurses -lm

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
