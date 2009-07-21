

/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <types.h>
#include <textcolor.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define PRINT_VALUE(color,string)  \
    color_on(BRIGHT,(color));      \
    printf(#string"\n");            \
    color_reset()

#define BOXWIDTH 4


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

BoxContainer*
asciiBoxes_allocateContainer(int numLines, int numColumns)
{
    int i;
    BoxContainer* container;

    container = (BoxContainer*) malloc(sizeof(BoxContainer));
    container->numLines = numLines;
    container->numColumns = numColumns;

    container->boxes = (box**) malloc(numLines * sizeof(Box*));

    for(i=0; i<numColumns; i++)
    {
        container->boxes[i] = (box*) malloc(numColumns * sizeof(Box));
    }

    return container;
}

void 
asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label)
{
    container->boxes[line][column]->width = 1;
    container->boxes[line][column]->label = (char*) malloc(strlen(label) * sizeof(char));
    strcpy(container->boxes[line][column]->label,label);
}


void
asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn,  char* label)
{
    container->boxes[line][column]->width = endColumn-startColumn;
    container->boxes[line][column]->label = (char*) malloc(strlen(label) * sizeof(char));
    strcpy(container->boxes[line][column]->label,label);
}

void
asciiBoxes_print(BoxContainer* container)
{
    int i;
    int j;

    printf("+");
    for (i=0; i< (container->numColumns * BOXWIDTH); i++)
    {
        printf("-");
    }
    printf("+\n");

    for (i=0; i<container->numLines; i++)
    {
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("+----+ ");
        }
        printf("|\n");
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("| %s | ",container->boxes[i][j].label);
        }
        printf("|\n");
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("+----+ ");
        }
        printf("|\n");
    }

    printf("+");
    for (i=0; i< container->numColumns; i++)
    {
        printf("-");
    }
    printf("+\n");
}




