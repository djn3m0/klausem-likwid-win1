#ifndef ASCIIBOXES_TYPES_H
#define ASCIIBOXES_TYPES_H

#include  <bstrlib.h>

typedef struct box {
    int width;
    bstring label;
} Box;

typedef struct boxContainer {
    int numLines;
    int numColumns;
    Box** boxes;
} BoxContainer;

#endif /*ASCIIBOXES_TYPES_H*/
