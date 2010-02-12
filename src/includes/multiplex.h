#ifndef MULTIPLEX_H
#define MULTIPLEX_H

#include <types.h>

extern void multiplex_init(MultiplexCollections* set);
extern void multiplex_start(int useMarker);
extern void multiplex_stop();

#endif /* MULTIPLEX_H */
