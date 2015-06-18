#ifndef THANOS_COMMON_
#define THANOS_COMMON_

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

extern FILE *output;

#define CYAN "\e[0;36m"
#define RED "\e[0;31m"
#define RESET "\e[0m"

#define GOOD_M "%s[+]%s "
#define BAD_M "%s[!]%s "

#define say(x...) fprintf(stdout, x);
#define warn(x...) fprintf(stderr, x);

#define file_print(x...) \
	if(output){\
		fprintf(output, x); \
	}

#define err(x...) \
	do { \
		warn(x); \
		perror(NULL); \
		exit(1); \
	} while(0);

#define good(x...) \
	say(GOOD_M, CYAN, RESET); \
	say(x);

#define bad(x...) \
	say(BAD_M, RED, RESET); \
	say(x);

#define die(x...) \
	do { \
		warn(x); \
		exit(1); \
	} while(0);

#endif
