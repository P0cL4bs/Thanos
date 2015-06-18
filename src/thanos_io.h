#ifndef THANOS_IO
#define THANOS_IO

//#ifndef MAX_SIZE // when i make a configure file, it will be set using ./configure --set-max-size xxxxx ???
#define MAX_SIZE 1024
//#endif

#define HEX_SIZE MAX_SIZE*2 +1


#include "thanos_common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int xopen(const char *filename, int mode);
size_t filesize(int fd);
void overwrite_file(const char *filename);
FILE *xfopen(const char *filename, const char *mode);

#endif
