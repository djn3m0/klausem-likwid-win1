CC  = icc
AS  = as
PAS = ./perl/AsmGen.pl 

CFLAGS   = -g -O3 -xW -align -fno-alias -std=c99 -vec-report0
ASFLAGS  = -g -gstabs
CPPFLAGS =
LFLAGS   = 
DEFINES  = -D_GNU_SOURCE

INCLUDES =
LIBS     = 


