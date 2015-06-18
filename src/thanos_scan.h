#ifndef THANOS_SCAN_H
#define THANOS_SCAN_H

#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

#include "thanos_common.h"
#include "thanos_mem.h"
#include "thanos_str.h"

typedef struct {
	int timeout;
	char *send_data;

	size_t data_size;
	size_t max_len;

	unsigned short port;
} THANOS_SCAN;

typedef struct {
	int len;
	THANOS_SCAN *scanner;
} THANOS;

extern THANOS thanos_scan;
extern bool nowide;
extern bool hexdump;
extern pthread_mutex_t mutexsum;

void *th4n0s_scanner(void *ptr);

#endif
