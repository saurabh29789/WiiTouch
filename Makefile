
ifeq ($(DEBUG), 1)
DEBUG_MODE   = -g -O0 -DWII_DEBUG
endif

CC      = /usr/bin/gcc
OBJS    = main.o wt_xdo.o queue.o \
			 wt_cwiid.o wt_sdl.o wt_types.o
SRC     = ./src
SOURCES = $(SRC)/main.c   $(SRC)/wt_xdo.c   \
			 $(SRC)/queue.c  $(SRC)/wt_cwiid.c \
			 $(SRC)/wt_sdl.c $(SRC)/wt_types.c

INCLUDES= -I /usr/include/ -I ./if/
LIBRARY = -lxdo -lcwiid -lSDL
CFLAGS  = -std=c99 -Wall $(DEBUG_MODE)

all: cwiid_app

cwiid_app: $(OBJS)
	$(CC) -o cwiid_app $(CFLAGS) $(INCLUDES) $(SOURCES) $(LIBRARY)

$(OBJS) : $(SOURCES)

clean: 
	rm cwiid_app
