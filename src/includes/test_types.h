#ifndef TEST_TYPES_H
#define TEST_TYPES_H

#include <stdint.h>
#include <bstrlib.h>

typedef void (*FuncPrototype)();

typedef enum {
    DataType_SINGLE = 0,
    DataType_DOUBLE} DataType;

typedef enum {
    STREAM_1 = 1,
    STREAM_2,
    STREAM_3,
    STREAM_4,
    STREAM_5,
    STREAM_6,
    STREAM_7,
    STREAM_8,
    STREAM_9,
    STREAM_10,
    STREAM_11,
    STREAM_12,
    MAX_STREAMS} Pattern;

typedef struct {
    char* name;
    Pattern streams;
    DataType type ;
    int stride;
    FuncPrototype kernel;
    int  flops;
    int  bytes;
} TestCase;

typedef struct {
    uint32_t   size;
    uint32_t   iter;
    const TestCase* test;
    uint64_t   cycles;
    int numberOfThreads;
    int* processors;
    void** streams;
} ThreadUserData;

#endif /*TEST_TYPES_H*/
