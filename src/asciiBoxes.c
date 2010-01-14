/*
 * ===========================================================================
 *
 *       Filename:  asciiBoxes.c
 *
 *    Description:  Module implementing output of nested ascii art boxes
 *
 *        Version:  1.0
 *        Created:  08/13/2009
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 */



/* #####   HEADER FILE INCLUDES   ######################################### */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <types.h>


/* #####   MACROS  -  LOCAL TO THIS SOURCE FILE   ######################### */

#define BOXWIDTH 6


/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */

BoxContainer*
asciiBoxes_allocateContainer(int numLines, int numColumns)
{
    int i;
    int j;
    BoxContainer* container;

    container = (BoxContainer*) malloc(sizeof(BoxContainer));
    container->numLines = numLines;
    container->numColumns = numColumns;

    container->boxes = (Box**) malloc(numLines * sizeof(Box*));

    for(i=0; i<numLines; i++)
    {
        container->boxes[i] = (Box*) malloc(numColumns * sizeof(Box));
    }

    for(i=0; i<numLines; i++)
    {
        for(j=0; j<numColumns; j++)
        {
            container->boxes[i][j].width = 0;
            container->boxes[i][j].label = NULL;
        }
    }


    return container;
}

void 
asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label)
{
    if( line >= container->numLines)
    {
        printf("ERROR: line id %d too large!\n",line);
        exit(EXIT_FAILURE);
    }
    if( column >= container->numColumns)
    {
        printf("ERROR: column id %d too large!\n",column);
        exit(EXIT_FAILURE);
    }

    if (container->boxes[line][column].label == NULL)
    {
        container->boxes[line][column].label = (char*) malloc(strlen(label) * sizeof(char));
    }

    container->boxes[line][column].width = 1;
    strcpy(container->boxes[line][column].label,label);
}


void
asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn,  char* label)
{
    if( line >= container->numLines)
    {
        printf("ERROR: line id %d too large!\n",line);
        exit(EXIT_FAILURE);
    }
    if( endColumn >= container->numColumns)
    {
        printf("ERROR: column id %d too large!\n",endColumn);
        exit(EXIT_FAILURE);
    }

    if (container->boxes[line][startColumn].label == NULL)
    {
        container->boxes[line][startColumn].label = (char*) malloc(strlen(label) * sizeof(char));
    }

    container->boxes[line][startColumn].width = (endColumn-startColumn)+1;
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
            width = (container->boxes[i][j].width * BOXWIDTH+(container->boxes[i][j].width-1)*3-6)/2;
            printf("|");

            for (k=0; k<width; k++)
            {
                printf(" ");
            }

            printf("%6s",container->boxes[i][j].label);

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




