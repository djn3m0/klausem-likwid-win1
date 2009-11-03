TARGET   = libptoverride.so

CC        = gcc
DEFINES   = -DCOLOR=CYAN    # Color the output

CFLAGS   = -O2 -Wall -std=c99 -shared -fPIC
CPPFLAGS = $(DEFINES) -D_GNU_SOURCE -I.
INCLUDES = -I../includes
LIBS     = -ldl -lpthread

all: $(TARGET)

$(TARGET): pthread-overload.c
	$(CC) $(CPPFLAGS) $(INCLUDES) $(CFLAGS) -o ../../$(TARGET) pthread-overload.c $(LIBS)

