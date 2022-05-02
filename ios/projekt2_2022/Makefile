CC          = gcc
CFLAGS		= -std=gnu99 -Wall -Wextra -Werror -pedantic -ggdb
LDFLAGS		= -lpthread -lrt
SOURCES		= main.c oxygen.c hydrogen.c barrier.c utils.c
INCLUDES	= oxygen.h hydrogen.h barrier.h utils.h globals.h
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= proj2

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c: .o
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

zip: $(SOURCES) $(INCLUDES) Makefile
	zip -r proj2.zip $^

test: $(TARGET)
	./$(TARGET) 1 2 1 1
	cat proj2.out
	wc -l proj2.out

clean:
	rm -rf $(OBJECTS) $(TARGET) proj2.out proj2.zip