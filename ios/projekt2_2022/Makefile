CC          = gcc
CFLAGS		= -std=gnu99 -Wall -Wextra -Werror -pedantic -ggdb
LDFLAGS		=
SOURCES		= main.c oxygen.c hydrogen.c barrier.c utils.c
INCLUDES	= 
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= proj2

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c: .o
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)