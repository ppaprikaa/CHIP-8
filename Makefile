SOURCE_FILES=*.c
TARGET=chip8
CC=clang
LIBS=-lSDL2

build:
	$(CC) -o $(TARGET) -Wall -Werror $(LIBS) $(SOURCE_FILES)

build-debug:
	$(CC) -o $(TARGET)_debug -g -Wall -Werror $(LIBS) $(SOURCE_FILES)
