TARGET=libel.so
CC=gcc
CFLAGS=-ggdb -o3 -w -fPIC
C_SOURCES=$(shell find . -name "*.c")
C_OBJECTS=$(patsubst %.c, %.o, $(C_SOURCES))
HEADERS=$(shell find . -name "*.h")
TEMP=$(shell find . -name "*~")

$(TARGET): $(C_OBJECTS)
	$(CC) -shared $^ -o $@

%.o: $(C_SOURCES) %.h
	$(CC) $(CFLAGS) -c $< -o $@

install:
	sudo cp $(TARGET) /usr/lib
	if [ ! -d "/usr/include/libel" ]; then sudo mkdir /usr/include/libel; fi;
	sudo cp $(HEADERS) /usr/include/libel
uninstall:
	sudo rm -rf /usr/include/libel
	sudo rm -f /usr/lib/libel.so

config:
	sh sys_compat.sh 1>/dev/null 2>&1

clean:
	rm -f $(shell find . -name "*.o") $(TEMP) $(TARGET) config.h
