#ifndef THANOS_MEM
#define THANOS_MEM

#include <stdlib.h>
#include <assert.h>

#include "thanos_common.h"

#define xfree(x) _xfree((void **)&x);

void *xmalloc(size_t size);
void _xfree(void **ptr);

#endif
