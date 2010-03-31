#ifndef ASCIITABLE_TYPES_H
#define ASCIITABLE_TYPES_H

#include  <bstrlib.h>

typedef struct bstrList bstrList; 

typedef struct {
    int numRows;
    int numColumns;
    int currentRow;
    int printed;
    bstrList*  header;
    bstrList** rows;
} TableContainer;


#endif /*ASCIITABLE_TYPES_H*/
