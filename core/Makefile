TARGET=el
CC=gcc
CFLAGS=-I include
C_SOURCES=$(shell find . -name "*.c")
C_OBJECTS=$(patsubst %.c, %.o, $(C_SOURCES))
HEADERS=$(shell find . -name "*.h")
TEMP=$(shell find . -name "*~")

$(TARGET): $(C_OBJECTS)
	$(CC) $^ -o $@

%.o: $(C_SOURCES) %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(shell find . -name "*.o") $(TEMP) $(TARGET) config.h
