CFLAGS= -ggdb
PROGRAMS= pulsemka pulsemka2 pulsemka3
SRCS= pulsemka.c pulsemka2.c pulsemka3.c
OBJS= $(SRCS:.c=.o)

all : $(PROGRAMS)

PROGRAMS : $(OBJS)

$(OBJS) : $(SRCS)

clean :
	rm $(PROGRAMS) $(OBJS)
