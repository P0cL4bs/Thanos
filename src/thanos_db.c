#include "thanos_db.h"

void create_db(const char *filename){

	sqlite3 *db;

	const char query[]=	"create table thanos_scan (\n"
				"	id INTEGER PRIMARY KEY,\n"
				"	port INTEGER NOT NULL,\n"
				"	data TEXT,\n"
				"	timeout INTEGER,\n"
				"	limit_download INTEGER\n"
				");";

	char x = 0;
	int ret = 0;

	printf("\n");
	good("Creating new database file: %s\n", filename);

	if( access(filename, F_OK) != -1 ){
		bad("file %s already exists\n", filename);

		do {
			printf("\nDo You want overwrite this file ? [y/n] ");
			x = getchar();
			while(getchar() != '\n');
		} while(x != 'y' && x != 'Y' && x != 'n' && x != 'N');
		printf("\n");
		if(x == 'y' || x == 'Y'){
			overwrite_file(filename);
		} else {
			bad("no database create\n");
			good("exiting database file creation\n\n");
			return;
		}
	}

	open_db(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, &db);
	good("Ok\n");

	good("Creating table\n");
	good("Executing query:\n\n%s\n\n", query);

	if( (ret = sqlite3_exec(db, query, 0, NULL, NULL)) != SQLITE_OK ){
		fprintf(stderr, "sqlite3_exec() error: %s\n", sqlite3_errstr(ret));
		exit(1);
	}

	good("Ok\n\n");

	sqlite3_close_v2(db);
}

int check_thanos_struct(sqlite3 *db){
	struct x {
		int id;
		char *name;
		char *type;
		int notnull;
		int pk;
	};

	const struct x table_struct[]=
	{
		{0,"id","INTEGER",0,1},
		{1,"port","INTEGER",1,0},
		{2,"data","TEXT",0,0},
		{3,"timeout","INTEGER",0,0},
		{4,"limit_download","INTEGER",0,0},
	};

	int i, len = 0, ret = 0;

	sqlite3_stmt *stmt;

	if( sqlite3_prepare_v2(db, "PRAGMA table_info(thanos_scan);", -1, &stmt, NULL) != SQLITE_OK){
		return 0;
	}

	len = sizeof(table_struct) / sizeof(struct x);

	for(i=0; i<len; i++){
		if(sqlite3_step(stmt) != SQLITE_ROW)
			break;

		// esse if tomou whey e ficou monstrão >

		if(table_struct[i].id != sqlite3_column_int(stmt, 0)
		|| strcmp(table_struct[i].name, (char *)sqlite3_column_text(stmt, 1))
		|| strcmp(table_struct[i].type, (char *)sqlite3_column_text(stmt, 2))
		|| table_struct[i].notnull != sqlite3_column_int(stmt, 3)
		|| table_struct[i].pk != sqlite3_column_int(stmt, 5)
		){
			break;
		}
	}

	if(i == len && sqlite3_step(stmt) != SQLITE_ROW)
		ret = 1;

	sqlite3_finalize(stmt);

	return ret;
}

void open_db(const char *filename, int flags, sqlite3 **db){
	int err = 0;
	if( (err = sqlite3_open_v2(filename, db, flags, NULL)) != SQLITE_OK){
		die("sqlite3_open_v2() error: %s\n", sqlite3_errstr(err));
	}
}

void delete_data(sqlite3 *db, int id){
	sqlite3_stmt *stmt;
	int rc;


	sqlite3_prepare_v2(db, "delete from thanos_scan where id = ?1;", -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, id);

	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE){
		die("error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);

}

void insert_data(sqlite3 *db, const char *file_data, int port, int timeout, int limit){
	sqlite3_stmt *stmt;

	char *data = NULL;
	char encode_data[HEX_SIZE];

	size_t size_file = 0;
	int fd = 0, rc = 0;


	if(file_data){
		fd = xopen(file_data, O_RDONLY);
		size_file = filesize(fd);
		if(size_file > MAX_SIZE){
			die("error: file is big than %d\n",MAX_SIZE);
		}

		data = mmap(0, size_file, PROT_READ, MAP_PRIVATE, fd, 0);
		hexencode(data, encode_data, size_file);
		munmap(data, size_file);
		close(fd);
	} else {
		encode_data[0] = 0;
	}

	sqlite3_prepare_v2(db, "insert into thanos_scan values(null, ?1, ?2, ?3, ?4);", -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, port);
	sqlite3_bind_text(stmt, 2, encode_data, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, timeout);
	sqlite3_bind_int(stmt, 4, limit);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE){
		die("error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);

}

void list_data(sqlite3 *db){
	sqlite3_stmt *stmt;
	int rc;

	sqlite3_prepare_v2(db, "select * from thanos_scan;", -1, &stmt, NULL);

	while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    	good("ID: %d\n", sqlite3_column_int(stmt, 0));
    	good("Port: %d\n", sqlite3_column_int(stmt, 1));
    	good("Data: %s\n", sqlite3_column_text(stmt, 2));
    	good("Timeout: %d\n", sqlite3_column_int(stmt, 3));
    	good("Limit Download: %d\n\n", sqlite3_column_int(stmt, 4));
	}

	sqlite3_finalize(stmt);
}


void update_int(sqlite3 *db, const char *query, int update_id, int new_value){
	sqlite3_stmt *stmt;
	int rc;

	sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 1, new_value);
	sqlite3_bind_int(stmt, 2, update_id);
	rc = sqlite3_step(stmt);

	if(rc!=SQLITE_DONE){
		die("error: %s\n", sqlite3_errmsg(db));
	}
	sqlite3_finalize(stmt);
}

void update_data_file(sqlite3 *db, int update_id, char *filename){
	sqlite3_stmt *stmt;

	char *data = NULL;
	char encode_data[HEX_SIZE];

	size_t size_file = 0;
	int fd = 0, rc = 0;


	if(filename){
		fd = xopen(filename, O_RDONLY);
		size_file = filesize(fd);

		if(size_file > MAX_SIZE){
			die("error: file is big than %d\n",MAX_SIZE);
		}

		data = mmap(0, size_file, PROT_READ, MAP_PRIVATE, fd, 0);
		hexencode(data, encode_data, size_file);
		munmap(data, size_file);
		close(fd);

	} else {
		encode_data[0] = 0;
	}

	sqlite3_prepare_v2(db, "update thanos_scan set data = ?1 where id = ?2;", -1, &stmt, NULL);
	sqlite3_bind_int(stmt, 2, update_id);
	sqlite3_bind_text(stmt, 1, encode_data, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);

	if(rc!=SQLITE_DONE){
		die("error: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);

}

void load_struct(sqlite3 *db){
	sqlite3_stmt *stmt;
	int count = 0, rc = 0, id = 0, bytes;

	sqlite3_prepare_v2(db, "select count(*) from thanos_scan;", -1, &stmt, NULL);
	rc = sqlite3_step(stmt);

	if(rc!=SQLITE_ROW){
		die("error: %s\n", sqlite3_errmsg(db));
	}

	count = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	if(!count){
		die("error: the database are blank\n");
	}

	thanos_scan.len = count;
	thanos_scan.scanner = xmalloc(count*sizeof(THANOS_SCAN));

	sqlite3_prepare_v2(db, "select * from thanos_scan;", -1, &stmt, NULL);



	while ( (rc = sqlite3_step(stmt)) == SQLITE_ROW){
		thanos_scan.scanner[id].timeout = sqlite3_column_int(stmt, 3);
		thanos_scan.scanner[id].max_len = sqlite3_column_int(stmt, 4);
		thanos_scan.scanner[id].port = (unsigned short) htons(sqlite3_column_int(stmt, 1));

		bytes = sqlite3_column_bytes(stmt, 2);

		if(bytes%2){
			die("error: invalida data at row id: %d\n",id+1);
		}

		bytes /= 2;
		thanos_scan.scanner[id].data_size = bytes;

		if(bytes){
			thanos_scan.scanner[id].send_data = xmalloc(bytes);
			hexdecode(sqlite3_column_text(stmt, 2), thanos_scan.scanner[id].send_data);
		} else {
			thanos_scan.scanner[id].send_data = NULL;
		}

		id++;
	}

	sqlite3_finalize(stmt);
}
