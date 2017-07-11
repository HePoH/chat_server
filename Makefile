.PHONY: prepare clean $(SUBDIRS)

vpath %.c src:lib
vpath %.h include
vpath %.o src:lib

CC = gcc
CFLAGS = -std=c99 -O2 -Wall -pedantic -ansi -g
LDFLAGS = -lrt -lpthread -lncurses -lform -lmenu

SUBDIRS = lib src
OBJECTS = core.o main.o

all: prepare $(OBJECTS)
		#clear
		$(CC) -o server $(OBJECTS) $(CFLAGS) $(LDFLAGS)
		./server

prepare: $(SUBDIRS)
$(SUBDIRS):
		make -C $@

src: lib

clean:
		clear
		rm -rf *.o server

