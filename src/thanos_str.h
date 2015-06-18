#ifndef THANOS_STR
#define THANOS_STR

#include <stdint.h>
#include <stdlib.h>

#include "thanos_common.h"

int th4n0s_inet_addr(const char *str, uint32_t *ret);
void th4n0s_hex(const char *x, int size);
void hexencode(const char *input, char *output, size_t len);
void hexdecode(const unsigned char *input, char *output);

#endif
