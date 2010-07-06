/*
 * ===========================================================================
 *
 *      Filename:  strUtil.c
 *
 *      Description:  Utility routines for strings. Depends on bstring lib.
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <types.h>
#include <bstrlib.h>
#include <strUtil.h>
#include <affinity.h>

int
str2int(const char* str)
{
    char* endptr;
    errno = 0;
    unsigned long val;
    val = strtoul(str, &endptr, 10);

    if ((errno == ERANGE && val == LONG_MAX )
            || (errno != 0 && val == 0)) 
    {
        perror("strtol");
        exit(EXIT_FAILURE);
    }

    if (endptr == str) 
    {
        fprintf(stderr, "No digits were found\n");
        exit(EXIT_FAILURE);
    }

    return (int) val;
}

int
bstr_to_cpuset(int* threads,  bstring q)
{
    int i;
    unsigned int rangeBegin;
    unsigned int rangeEnd;
    int numThreads=0;
    struct bstrList* tokens;
    struct bstrList* subtokens;
//    bstring q = bfromcstr(str);

    tokens = bsplit(q,',');

    for (i=0;i<tokens->qty;i++)
    {
        subtokens = bsplit(tokens->entry[i],'-');

        if (numThreads > MAX_NUM_THREADS)
        {
            fprintf(stderr, "Number Of threads too large!\n");
            exit(EXIT_FAILURE);
        }

        if( subtokens->qty == 1 )
        {
            threads[numThreads] = str2int((char *) subtokens->entry[0]->data);
            numThreads++;
        }
        else if ( subtokens->qty == 2 )
        {
            rangeBegin = str2int((char*) subtokens->entry[0]->data);
            rangeEnd = str2int((char*) subtokens->entry[1]->data);

            if (!(rangeBegin <= rangeEnd))
            {
                fprintf(stderr, "Range End bigger than begin\n");
                exit(EXIT_FAILURE);
            }

            while (rangeBegin <= rangeEnd) {
                threads[numThreads] = rangeBegin;
                numThreads++;
                rangeBegin++;
            }
        }
        else
        {
            fprintf(stderr, "Parse Error\n");
            exit(EXIT_FAILURE);
        }
        bstrListDestroy(subtokens);
    }

    bstrListDestroy(tokens);
    bdestroy(q);

    return numThreads;
}


void
bstr_to_eventset(StrUtilEventSet* set, bstring q)
{
    int i;
    struct bstrList* tokens;
    struct bstrList* subtokens;

    tokens = bsplit(q,',');
    set->numberOfEvents = tokens->qty;
    set->events = (StrUtilEvent*)
        malloc(set->numberOfEvents * sizeof(StrUtilEvent));

    for (i=0;i<tokens->qty;i++)
    {
        subtokens = bsplit(tokens->entry[i],':');

        if ( subtokens->qty != 2 )
        {
            fprintf(stderr, "Error in parsing event string\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            set->events[i].eventName = bstrcpy(subtokens->entry[0]);
            set->events[i].counterName = bstrcpy(subtokens->entry[1]);
        }

        bstrListDestroy(subtokens);
    }

    bstrListDestroy(tokens);
    bdestroy(q);
}

int bstr_to_doubleSize(bstring str, DataType type)
{
    bstring unit = bmidstr(str, blength(str)-2, 2);
    bstring sizeStr = bmidstr(str, 0, blength(str)-2);
    int sizeU = str2int(bdata(sizeStr));
    int junk = 0;
    int bytesize = 0;

    switch (type)
    {
        case SINGLE:
            bytesize = 4;
            break;

        case DOUBLE:
            bytesize = 8;
            break;
    }

    if (biseqcstr(unit, "kB")) {
        junk = (sizeU *1024)/bytesize;
    } else if (biseqcstr(unit, "MB")) {
        junk = (sizeU *1024*1024)/bytesize;
    } else if (biseqcstr(unit, "GB")) {
        junk = (sizeU *1024*1024*1024)/bytesize;
    }

    return junk;
}

void bstr_to_workgroup(Workgroup* group,
        bstring str,
        DataType type,
        int numberOfStreams)
{
    int i;
    int parseStreams = 0;
    bstring threadInfo;
    bstring streams= bformat("0");
    struct bstrList* tokens;
    struct bstrList* subtokens;
    const AffinityDomain* domain;

    /* split the workgroup into the thread and the streams part */
    tokens = bsplit(str,'-');

    if (tokens->qty == 2)
    {
        threadInfo = bstrcpy(tokens->entry[0]);
        streams = bstrcpy(tokens->entry[1]);
        parseStreams = 1;
    } 
    else if (tokens->qty == 1)
    {
        threadInfo = bstrcpy(tokens->entry[0]);
    }
    else
    {
        fprintf(stderr, "Error in parsing workgroup string\n");
        exit(EXIT_FAILURE);
    }

    bstrListDestroy (tokens);
    tokens = bsplit(threadInfo,':');

    if (tokens->qty == 3)
    {
        domain = affinity_getDomain(tokens->entry[0]);
        group->size = bstr_to_doubleSize(tokens->entry[1], type);
        group->numberOfThreads = str2int(bdata(tokens->entry[2]));

        if (group->numberOfThreads > domain->numberOfProcessors)
        {
            fprintf(stderr, "Error: Domain %s supports only up to %d threads.\n",
                    bdata(tokens->entry[0]),domain->numberOfProcessors);
            exit(EXIT_FAILURE);
        }

        group->processorIds = (int*) malloc(group->numberOfThreads * sizeof(int));

        for (i=0; i<group->numberOfThreads; i++)
        {
            group->processorIds[i] = domain->processorList[i];
        }
    } 
    else if (tokens->qty == 2)
    {
        domain = affinity_getDomain(tokens->entry[0]);
        group->size = bstr_to_doubleSize(tokens->entry[1], type);
        group->numberOfThreads = domain->numberOfProcessors;

        group->processorIds = (int*) malloc(group->numberOfThreads * sizeof(int));

        for (i=0; i<group->numberOfThreads; i++)
        {
            group->processorIds[i] = domain->processorList[i];
        }
    }
    else
    {
        fprintf(stderr, "Error in parsing workgroup string\n");
        exit(EXIT_FAILURE);
    }

    bstrListDestroy(tokens);

    /* parse stream list */
    if (parseStreams)
    {
        tokens = bsplit(streams,',');

        if (tokens->qty < numberOfStreams)
        {
            fprintf(stderr, "Error: Testcase requires at least %d streams.\n",
                    numberOfStreams);
            exit(EXIT_FAILURE);
        }

        group->streams = (Stream*) malloc(numberOfStreams * sizeof(Stream));

        for (i=0;i<tokens->qty;i++)
        {
            subtokens = bsplit(tokens->entry[i],':');

            if ( subtokens->qty == 3 )
            {
               int index = str2int(bdata(subtokens->entry[0]));
               if (index >= numberOfStreams)
               {
                   fprintf(stderr, "Error: Stream Index %d out of range.\n", index);
                   exit(EXIT_FAILURE);
               }
               group->streams[index].domain = bstrcpy(subtokens->entry[1]);
               group->streams[index].offset = str2int(bdata(subtokens->entry[2]));
            }
            else if ( subtokens->qty == 2 )
            {
                int index = str2int(bdata(subtokens->entry[0]));
                if (index >= numberOfStreams)
                {
                    fprintf(stderr, "Error: Stream Index %d out of range.\n", index);
                    exit(EXIT_FAILURE);
                }
                group->streams[index].domain = bstrcpy(subtokens->entry[1]);
                group->streams[index].offset = 0;
            }
            else
            {
                fprintf(stderr, "Error in parsing event string\n");
                exit(EXIT_FAILURE);
            }

            bstrListDestroy(subtokens);
        }

        bstrListDestroy(tokens);
        bdestroy(str);
    }
    else
    {
        group->streams = (Stream*) malloc(numberOfStreams * sizeof(Stream));

        for (i=0; i<numberOfStreams; i++)
        {
            group->streams[i].domain = domain->tag;
            group->streams[i].offset = 0;
        }
    }

    group->size /= numberOfStreams;
}


#define INIT_SECURE_INPUT_LENGTH 256

bstring
bSecureInput (int maxlen, char* vgcCtx) {
    int i, m, c = 1;
    bstring b, t;
    int termchar = 0;

    if (!vgcCtx) return NULL;

    b = bfromcstralloc (INIT_SECURE_INPUT_LENGTH, "");

    for (i=0; ; i++)
    {
        if (termchar == c)
        {
            break;
        }
        else if ((maxlen > 0) && (i >= maxlen))
        {
            b = NULL;
            return b;
        }
        else
        {
            c = *(vgcCtx++);
        }

        if (EOF == c)
        {
            break;
        }

        if (i+1 >= b->mlen) {

            /* Double size, but deal with unusual case of numeric
               overflows */

            if ((m = b->mlen << 1)   <= b->mlen     &&
                    (m = b->mlen + 1024) <= b->mlen &&
                    (m = b->mlen + 16)   <= b->mlen &&
                    (m = b->mlen + 1)    <= b->mlen)
            {
                t = NULL;
            }
            else
            {
                t = bfromcstralloc (m, "");
            }

            if (t)
            {
                memcpy (t->data, b->data, i);
            }

            bdestroy (b); /* Clean previous buffer */
            b = t;
            if (!b)
            {
                return b;
            }
        }

        b->data[i] = (unsigned char) c;
    }

    i--;
    b->slen = i;
    b->data[i] = (unsigned char) '\0';
    return b;
}


int bJustifyCenter (bstring b, int width) 
{
    unsigned char space  = ' ';
    int alignSpace = (width - b->slen) / 2;
    int restSpace = (width - b->slen) % 2;
    if (width <= 0) return -__LINE__;

    if (b->slen <= width)
    {
        binsertch (b, 0, alignSpace, space);
    }

    binsertch (b, b->slen , alignSpace+restSpace, space);

    return BSTR_OK;
}


