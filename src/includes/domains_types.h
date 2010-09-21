#ifndef AFFINITY_TYPES_H
#define AFFINITY_TYPES_H

typedef struct {
    bstring tag;
    int numberOfProcessors;
    int*  processorList;
} AffinityDomain;


#endif /*AFFINITY_TYPES_H*/
