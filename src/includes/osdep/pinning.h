#ifndef PINNING_H_ghe873
#define PINNING_H_ghe873

extern int  pinning_processGetProcessorId();
extern int  pinning_threadGetProcessorId();
extern int  pinning_pinProcess(int processorId);

extern int  pinning_pinThread(int processorId);

#endif
