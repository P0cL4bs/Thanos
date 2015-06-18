#ifndef _THANOS_DB
#define _THANOS_DB

#include <sqlite3.h>
#include <unistd.h>
#include <string.h>

#include "thanos_common.h"
#include "thanos_mem.h"
#include "thanos_io.h"
#include "thanos_str.h"
#include "thanos_scan.h"

#include <sys/mman.h>

void open_db(const char *filename, int flags, sqlite3 **db);
void overwrite_file(const char *filename);
void create_db(const char *filename);
int check_thanos_struct(sqlite3 *db);
void insert_data(sqlite3 *db, const char *file_data, int port, int timeout, int limit);
void delete_data(sqlite3 *db, int id);
void list_data(sqlite3 *db);

void update_int(sqlite3 *db, const char *query, int update_id, int new_value);
void update_data_file(sqlite3 *db, int update_id, char *filename);

void load_struct(sqlite3 *db);

#endif
