CC          = g++
CFLAGS		= -c -Wall
LDFLAGS		= -lpcap
SOURCES		= main.cpp
INCLUDES	= -I. -I./lib
OBJECTS		= $(SOURCES:.cpp=.o)
TARGET		= ipk-sniffer

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)