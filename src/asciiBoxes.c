

/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <types.h>
#include <textcolor.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define PRINT_VALUE(color,string)  \
    color_on(BRIGHT,(color));      \
    printf(#string"\n");            \
    color_reset()


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

BoxContainer*
asciiBoxes_allocateContainer(int numLines, int numColumns)
{
    BoxContainer* container;

    return container;
}

void 
asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label)
{
}


void
asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn,  char* label)
{
}

