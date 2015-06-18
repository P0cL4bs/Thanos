#include "thanos_mem.h"

void *xmalloc(size_t size){
	void *ptr = malloc(size);

	if(ptr == NULL){
		err("malloc() failed\n");
	}

	return ptr;
}

void _xfree(void **ptr){
	assert(ptr);
	if(ptr != NULL){
		free(*ptr);
		*ptr = NULL;
	}
}
