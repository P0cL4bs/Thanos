#include "thanos_io.h"

int xopen(const char *filename, int mode){
	int fd = open(filename, mode);

	if(fd < 0){
		err("open(%s, %d) failed: ",filename, mode);
	}

	return fd;
}

size_t filesize(int fd){
	struct stat buf;

	if( fstat(fd, &buf) == -1 ){
		err("fstat(%d, &buf) failed: ", fd);
	}

	return (size_t) buf.st_size;
}

void overwrite_file(const char *filename){
	FILE *x;
	x = fopen(filename, "w+");
	fclose(x);
}

FILE *xfopen(const char *filename, const char *mode){
	FILE *ret = fopen(filename, mode);
	if(ret == NULL){
		err("fopen(%s, %s) failed: ", filename, mode);
	}

	return ret;
}