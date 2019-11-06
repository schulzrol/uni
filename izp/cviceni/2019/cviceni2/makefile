CFLAGS=-std=c99 -Wall -Wextra -pedantic -g
PROGRAM=cv2
SRCS=cv2.c
OBJS=$(SRCS:.c=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJS)

clean:
	-rm $(PROGRAM) $(OBJS)
