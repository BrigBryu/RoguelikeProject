CC = g++
# Add debugging symbols, disable optimization for better debugging, and enable all warnings
CFLAGS = -g3 -O0 -Wall -Wextra -pedantic -I$(GAMEOBJECTS_DIR) -I$(UTIL_DIR)

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
      $(GAMEOBJECTS_DIR)/npc.cpp \
      $(GAMEOBJECTS_DIR)/object.cpp

OBJ = $(SRC:.cpp=.o)
TARGET = assignment08

# Add debug target
.PHONY: debug
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lncurses -lm

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

# Add gdb target for quick debugging
.PHONY: gdb
gdb: debug
	gdb --args ./$(TARGET)
