CFLAGS=-std=c99 -Wall -Wextra -Werror -g

PROGRAM=proj2
SRCS=proj2.c

OBJS=$(SRCS:.c=.o)

	
$(PROGRAM): $(OBJS) -lm

all: $(PROGRAM)

clean:
	-rm $(PROGRAM) $(OBJS)
