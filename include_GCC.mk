CC  = gcc
AS  = as
AR  = ar
PAS = ./perl/AsmGen.pl 
GEN_PAS = ./perl/generatePas.pl 

ANSI_CFLAGS  = -ansi
ANSI_CFLAGS += -std=c99
ANSI_CFLAGS += -pedantic
ANSI_CFLAGS += -Wextra

CFLAGS   = -O1 -Wno-format  -Wall
ASFLAGS  = -g -gstabs
CPPFLAGS =
LFLAGS   = -pthread 
DEFINES  = -D_GNU_SOURCE
DEFINES  += -DMAX_NUM_THREADS=128
DEFINES  += -DPAGE_ALIGNMENT=4096

INCLUDES =
LIBS     = 


