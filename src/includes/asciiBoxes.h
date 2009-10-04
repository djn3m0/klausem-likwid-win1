/*
 * ===========================================================================
 *
 *       Filename:  asciiBoxes.h
 *
 *    Description:  Header File cpuid Module. 
 *                  Reads out cpuid information and initilaizes a global 
 *                  data structure cpuid_info.
 *
 *        Version:  1.0
 *        Created:  07/15/2009
 *       Revision:  none
 *
 *         Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *        Company:  RRZE Erlangen
 *        Project:  none
 *      Copyright:  Copyright (c) 2009, Jan Treibig
 *
 * ===========================================================================
 */

#ifndef ASCIIBOXES_H
#define ASCIIBOXES_H
#include <types.h>

extern BoxContainer* asciiBoxes_allocateContainer(int numLines,int numColumns);
extern void asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label);
extern void asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn, char* label);
extern void asciiBoxes_print(BoxContainer* container);

#endif /*ASCIIBOXES_H*/
