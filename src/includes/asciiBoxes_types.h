#ifndef ASCIIBOXES_TYPES_H
#define ASCIIBOXES_TYPES_H

typedef struct box {
    int width;
    char* label;
} Box;

typedef struct boxContainer {
    int numLines;
    int numColumns;
    Box* boxes;
} BoxContainer;

#endif /*ASCIIBOXES_TYPES_H*/
