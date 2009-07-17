TARGET_PT   = libptoverride_pt.so
TARGET_OMP  = libptoverride_omp.so

CC        = gcc
DEFINES   = -DCOLOR=CYAN    # Color the output
OMP_TYPE  = -DINTEL_OMP     # INTEL or GCC

CFLAGS   = -O2 -Wall -std=c99 -shared -fPIC
CPPFLAGS = $(DEFINES) -D_GNU_SOURCE -I.
INCLUDES = -I../includes
LIBS     = -ldl -lpthread

all: $(TARGET_PT)

$(TARGET_PT): pthread-overload.c
	$(CC) $(CPPFLAGS) $(INCLUDES) $(CFLAGS) -o ../../$(TARGET_PT) pthread-overload.c $(LIBS)

$(TARGET_OMP): pthread-overload.c
	$(CC) $(CPPFLAGS) $(INCLUDES) $(OMP_TYPE) $(CFLAGS) -o ../../$(TARGET_OMP) pthread-overload.c $(LIBS)

