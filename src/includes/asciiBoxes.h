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

#ifndef ASCIIBOXES_H
#define ASCIIBOXES_H
#include <types.h>

extern BoxContainer* asciiBoxes_allocateContainer(int numLines,int numColumns);
extern void asciiBoxes_addBox(BoxContainer* container, int line, int column, char* label);
extern void asciiBoxes_addJoinedBox(BoxContainer* container, int line, int startColumn, int endColumn, char* label);
extern void asciiBoxes_print(BoxContainer* container);

#endif /*ASCIIBOXES_H*/
