CC      = gcc
CFLAGS  = -O3 -Wall -Wextra -ggdb
TARGET  = yat
PREFIX  = /usr/local
INCLUDE = ./thirdparty

all: $(TARGET)

$(TARGET): yat.o
	$(CC) $(CFLAGS) -o $@ $^ -I$(INCLUDE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE)

da_test: thirdparty/da.h thirdparty/da_test.c
	gcc -o da_test thirdparty/da_test.c -I./thirdparty -ggdb

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET) *.o

