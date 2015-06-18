#ifndef THANOS_H
#define THANOS_H

#define THANOS_VERSION "1.0.0 beta"
#define THANOS_AUTHOR "MMxM" // <o.o>

#define OPTS "ht:o:ns:e:D"

#define MIN_TIMEOUT 1
#define MAX_TIMEOUT 100

#define DEFAULT_TIMEOUT 10
#define DEFAULT_LIMIT_BYTES 300

typedef struct {
	unsigned int threads;
	char *config;
	char *output_filename;
	char *data_file;
	uint32_t start;
	uint32_t end;
	bool list;
	int limit_bytes;
	int port;
	int timeout;
	int delete_id;
	int update_id;
} THANOS_OPTIONS;

enum {
	CHECK_START,
	CHECK_END,
	CHECK_PORT,
	CHECK_TIMEOUT,
	CHECK_LIMIT,
	CHECK_LEN
};

#endif

