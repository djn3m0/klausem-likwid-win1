

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

#define BOXWIDTH 5


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

BoxContainer*
asciiBoxes_allocateContainer(int numLines, int numColumns)
{
    int i;
    BoxContainer* container;

    container = (BoxContainer*) malloc(sizeof(BoxContainer));
    container->numLines = numLines;
    container->numColumns = numColumns;

    container->boxes = (Box**) malloc(numLines * sizeof(Box*));

    for(i=0; i<numColumns; i++)
    {
        container->boxes[i] = (Box*) malloc(numColumns * sizeof(Box));
    }

    return container;
}

void 
asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label)
{
    container->boxes[line][column].width = 1;
    container->boxes[line][column].label = (char*) malloc(strlen(label) * sizeof(char));
    strcpy(container->boxes[line][column].label,label);
}


void
asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn,  char* label)
{
    container->boxes[line][startColumn].width = endColumn-startColumn;
    container->boxes[line][startColumn].label = (char*) malloc(strlen(label) * sizeof(char));
    strcpy(container->boxes[line][startColumn].label,label);
}

void
asciiBoxes_print(BoxContainer* container)
{
    int i;
    int j;
    int k;
    int width;

    printf("+");
    for (i=0; i< (container->numColumns * (BOXWIDTH+2)+container->numColumns+1); i++)
    {
        printf("-");
    }
    printf("+\n");

    for (i=0; i<container->numLines; i++)
    {
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("+");
            if(container->boxes[i][j].width == 1)
            {
                for (k=0; k<BOXWIDTH; k++)
                {
                    printf("-");
                }
            }
            else 
            {
                for (k=0; k<(container->boxes[i][j].width * BOXWIDTH+(container->boxes[i][j].width-1)*3); k++)
                {
                    printf("-");
                }
                j+= container->boxes[i][j].width-1;
            }
            printf("+ ");
        }
        printf("|\n");
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("|");
            width = (container->boxes[i][j].width * BOXWIDTH+(container->boxes[i][j].width-1)*3-5)/2;
            for (k=0; k<width; k++)
            {
                printf(" ");
            }
            printf("%5s",container->boxes[i][j].label);
            for (k=0; k<width; k++)
            {
                printf(" ");
            }
            printf("| ");

            if(container->boxes[i][j].width != 1)
            {
                j+= container->boxes[i][j].width-1;
            }

        }
        printf("|\n");
        printf("| ");
        for (j=0; j<container->numColumns; j++)
        {
            printf("+");
            if(container->boxes[i][j].width == 1)
            {
                for (k=0; k<BOXWIDTH; k++)
                {
                    printf("-");
                }
            }
            else 
            {
                for (k=0; k<(container->boxes[i][j].width * BOXWIDTH+(container->boxes[i][j].width-1)*3); k++)
                {
                    printf("-");
                }
                j+= container->boxes[i][j].width-1;
            }
            printf("+ ");

        }
        printf("|\n");
    }

    printf("+");
    for (i=0; i< (container->numColumns * (BOXWIDTH+2)+container->numColumns+1); i++)
    {
        printf("-");
    }
    printf("+\n");
}




