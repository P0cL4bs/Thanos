/*
 -- Thanos Banner Grabber --

 Author: 	MMxM
 Github: 	https://github.com/hc0d3r

 Greetz: 	Mh4x0f, Joridos, n4sss, kwrnel, Dennis, Cooler, Kodo no kami, 
 			cyclone, susp31t0virtual, xstpl, m0nad, Sir.Rafiki, clandestine,
 			Janissaries Team

 P0cl4bs Team 2015 - ~

*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "thanos.h"
#include "thanos_db.h"
#include "thanos_str.h"
#include "thanos_common.h"
#include "thanos_scan.h"

void banner(void){
	printf("\n");
	printf("_____  _      __    _      ___   __  \n");
	printf(" | |  | |_|  / /\\  | |\\ | / / \\ ( (` \n");
	printf(" |_|  |_| | /_/--\\ |_| \\| \\_\\_/ _)_) \n");
	printf("\n");
	printf("       	Banner Grabber v%s\n\n",THANOS_VERSION);
}

void help(void){
	printf("\n");
	printf("  -h, --help                 Display this help menu\n\n");
	printf(" DB Options:\n\n");
	
	printf("  --new-db STRING            Create new database file\n");
	printf("  --cfg-db STRING            Set db configuration file\n");
	printf("  --cfg-port NUMBER          Insert new port on cfg-db\n");
	printf("  --cfg-file STRING          File contains the data that will be send on cfg-port. Default=NULL\n");
	printf("  --cfg-timeout NUMBER       Define timeout for connection. Default=%d\n",DEFAULT_TIMEOUT);
	printf("  --cfg-limit NUMBER         Max size of bytes to reached. Default=%d\n",DEFAULT_LIMIT_BYTES);
	printf("  --delete-id NUMBER         Delete a row in database\n");
	printf("  --update-id NUMBER         Update a row in database\n");
	printf("  --list                     List rows in database\n\n");


	printf(" Scan Options:\n\n");
	printf("  -s, --start-ip STRING      IP number to start scan\n");
	printf("  -e, --end-ip STRING        IP number to end scan\n");
	//printf("  -o, --output STRING        Output file\n"); depois eu faco isso
	printf("  -t, --threads NUMBER       Simultaneous connections number\n");
	printf("  -n, --no-wide              No print wide chars\n");
	printf("  -D, --hex-dump             Print output in hexdump format\n");

	printf("\n");
	exit(0);
}

void parser_opts(int argc, char **argv, THANOS_OPTIONS *opts){
	static struct option long_options[] =
	{
		{"help", no_argument, 0, 'h'}, //ok
		{"threads", required_argument, 0, 't'}, //ok

		{"output", required_argument, 0, 'o'}, //ok
		{"start-ip", required_argument, 0, 's'}, //ok
		{"end-ip", required_argument, 0, 'e'}, //ok
		{"no-wide", no_argument, 0, 'n'}, //ok
		{"hex-dump", no_argument, 0, 'D'}, //ok
		
		{"new-db", required_argument, 0, 0}, //ok
		{"cfg-db", required_argument, 0, 0}, //ok
		{"cfg-port", required_argument, 0, 0}, //ok
		{"cfg-file", required_argument, 0, 0}, //ok
		{"cfg-timeout", required_argument, 0, 0}, //ok
		{"cfg-limit", required_argument, 0, 0}, //ok

		{"delete-id", required_argument, 0, 0}, //ok
		{"update-id", required_argument, 0, 0}, //ok

		{"list", no_argument, 0, 0}, //ok
		{0, 0, 0, 0}
	};

	int Getopts, tmp, option_index = 0;
	static bool check[CHECK_LEN];

	char *opt_ptr;

	while(1){
		Getopts = getopt_long(argc, argv, OPTS, long_options, &option_index);

		if(Getopts == -1){
			break;
		}

		opt_ptr = (char *) long_options[option_index].name;

		switch(Getopts){
			case 0:
				if(!strcmp("new-db", opt_ptr)){
					create_db(optarg);
				}

				else if(!strcmp("cfg-db", opt_ptr)){
					opts->config = optarg;
				}
				
				else if(!strcmp("cfg-port", opt_ptr)){
					tmp = (int) strtol(optarg, NULL, 10);
					if(tmp < 0 || tmp > 0xffff){
						die("error: --cfg-port %s not is a valid port !!!\n", optarg);
					} else {
						opts->port = tmp;
						check[CHECK_PORT] = true;
					}
				}

				else if(!strcmp("cfg-timeout", opt_ptr)){
					tmp = (int) strtol(optarg, NULL, 10);
					if(tmp < MIN_TIMEOUT || tmp > MAX_TIMEOUT){
						die("error: --cfg-timeout %s not is a valid value\n", optarg);
					} else {
						opts->timeout = tmp;
						check[CHECK_TIMEOUT] = true;
					}
				}

				else if(!strcmp("cfg-limit", opt_ptr)){
					tmp = (int) strtol(optarg, NULL, 10);
					if(tmp < 0){
						die("error: --cfg-limit %s not is a valide value\n", optarg);
					} else {
						opts->limit_bytes = tmp;
						check[CHECK_LIMIT] = true;
					}
				}

				else if(!strcmp("cfg-file", opt_ptr)){
					opts->data_file = optarg;
				}

				else if(!strcmp("delete-id", opt_ptr)){
					tmp = (int) strtol(optarg, NULL, 10);
					if(tmp < 1){
						die("error: --delete-id %s not is a valide value\n", optarg);
					} else {
						opts->delete_id = tmp;
					}
				}

				else if(!strcmp("update-id", opt_ptr)){
					tmp = (int) strtol(optarg, NULL, 10);
					if(tmp < 1){
						die("error: --update-id %s not is a valide value\n", optarg);
					} else {
						opts->update_id = tmp;
					}
				}

				else if(!strcmp("list", opt_ptr)){
					opts->list = true;
				}

				break;

			case 'h':
				help();
				break;

			case 't':
				tmp = (int) strtol(optarg, NULL, 10);
				if(tmp < 0){
					die("-t, --threads error: %s not is a valide value\n", optarg);
				} else {
					opts->threads = (unsigned int)tmp;
				}
				break;

			case 'c':
				opts->config = optarg;
				break;

			case 'o':
				opts->output_filename = optarg;
				break;

			case 's':
				if( !th4n0s_inet_addr( optarg, &(opts->start) ) ){
					die("-s, --start error: %s not is a valide IP address\n", optarg);
				}
				check[CHECK_START] = true;
				break;

			case 'e':
				if( !th4n0s_inet_addr( optarg, &(opts->end) ) ){
					die("-e, --end error: %s not is a valide IP address\n", optarg);
				}
				check[CHECK_END] = true;
				break;

			case 'n':
				nowide = true;
				break;

			case 'D':
				hexdump = true;
				break;
			default:
				abort();
		}


	}

	sqlite3 *db;

	if(opts->config){
		open_db(opts->config, SQLITE_OPEN_READWRITE, &db);

		if(!check_thanos_struct(db)){
			die("%s not is a valide database\n", opts->config);
		}

		if(opts->delete_id){
			good("Deleting row, id: %d\n",opts->delete_id);
			delete_data( db, opts->delete_id );
			good("Deleted !!!\n\n");
		}

		if(opts->update_id){
			good("Updating row id: %d\n",opts->update_id);

			if(check[CHECK_PORT]){
				update_int(db, "update thanos_scan set port = ?1 where id = ?2;", opts->update_id, opts->port);
			}

			if(check[CHECK_TIMEOUT]){
				update_int(db, "update thanos_scan set timeout = ?1 where id = ?2;", opts->update_id, opts->timeout);
			}

			if(check[CHECK_LIMIT]){
				update_int(db, "update thanos_scan set limit_download = ?1 where id = ?2;", opts->update_id, opts->limit_bytes);
			}

			if(opts->data_file){
				update_data_file(db, opts->update_id, opts->data_file);
			}

			good("Update finish\n\n");
		} else {

			if(check[CHECK_PORT]){
				good("Inserting new options to database file\n");
				insert_data( db, opts->data_file, opts->port, opts->timeout, opts->limit_bytes ); //ok
				good("Ok\n\n");
			}
		}

		if(opts->list){
			good("Starting listing rows in database file\n\n");
			list_data(db);
			good("Finish\n\n");
		}
	} else {
		if(check[CHECK_PORT] || check[CHECK_START] || 
			check[CHECK_END] || opts->delete_id || opts->list || 
			opts->update_id){
			die("error: cfg-db not set\n");
		}

	}

	if(!check[CHECK_START] || !check[CHECK_END]){
		if(opts->config){
			sqlite3_close_v2(db);
		}

		exit(0);
	}

	if(opts->end < opts->start){
		if(opts->config){
			sqlite3_close_v2(db);
		}

		die("error: start address must be minor than end address\n");
	} else {
		load_struct(db);
		sqlite3_close_v2(db);
	}
}

void init_values(THANOS_OPTIONS *x){
	x->threads = 0;
	x->config = NULL;
	x->timeout = DEFAULT_TIMEOUT;
	x->output_filename = NULL;
	x->start = 0;
	x->end = 0;
	x->limit_bytes = DEFAULT_LIMIT_BYTES;
	hexdump = false;
	nowide = false;
	x->data_file = NULL;
	x->port = 0;
	x->delete_id = 0;
	x->list = false;
	x->update_id = 0;
}

int main(int argc, char *argv[]){
	THANOS_OPTIONS options;

	pthread_t *threads = NULL;
	uint32_t i_addr;
	unsigned int tid = 0, count = 0;
	int i;

	if(argc == 1){
		warn("thanos: try 'thanos -h' or 'thanos --help'\n");
		return 0;
	}

	banner();
	init_values(&options);
	parser_opts(argc, argv, &options);

	printf("Number of scans load: %d\n",thanos_scan.len);
	printf("Total IP's to scan: %u\n\n", options.end - options.start + 1);


	if(!options.threads){
		for(i_addr=options.start; i_addr<= options.end; i_addr++){
			th4n0s_scanner((void *) (uintptr_t) htonl(i_addr));
		}
	} else {
		threads = xmalloc(options.threads * sizeof(pthread_t));

		for(i_addr=options.start; i_addr <= options.end; i_addr++){
			if (pthread_create(&threads[tid], NULL, th4n0s_scanner, (void *) (uintptr_t) (htonl(i_addr))) != 0){
				err("error: pthread_create failed\n");
			}
			tid++;

			if(tid == options.threads){
				for(count=0; count < tid; count++){
					pthread_join(threads[count], NULL);
				}
				tid = 0;
			}
		}
	

		for(count=0; count < tid; count++){
			pthread_join(threads[count], NULL);
		}

		xfree(threads);
	}

	for(i=0; i < thanos_scan.len; i++){
		xfree(thanos_scan.scanner[i].send_data);
	}

	xfree(thanos_scan.scanner);

	return 0;
}

