CC=gcc
CFLAGS=-Wall -O3
INCLUDE=-I/usr/local/include
LDFLAGS=-L/usr/local/lib
LIBS=-levent
SRCS=main.c options.c clients.c client.c mtime.c
OBJS=$(SRCS:.c=.o)
MAIN=pounce

.PHONY: clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LDFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)
