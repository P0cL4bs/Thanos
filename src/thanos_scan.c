#include "thanos_scan.h"

THANOS thanos_scan;
bool nowide;
bool hexdump;
pthread_mutex_t mutexsum;

void *th4n0s_scanner(void *ptr){
	uint32_t ip_addr = (uint32_t) (uintptr_t)ptr;

	int sockfd = 0, i = 0, recv_ret = 0, j = 0;
	struct sockaddr_in target;
	char *recv_buffer;

	struct timeval timeout;
	timeout.tv_usec = 0;

	target.sin_addr.s_addr = ip_addr;
	target.sin_family = PF_INET;

	for(i=0; i < thanos_scan.len; i++){
		target.sin_port = thanos_scan.scanner[i].port;
		

		if((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_IP)) == -1){
			err("socket creation failed\n");
		}

		if(connect(sockfd, (struct sockaddr * ) &target, sizeof(target)) == -1){
			pthread_mutex_lock(&mutexsum);
			bad("%d.%d.%d.%d:%d closed\n", (ip_addr & 0xff), (ip_addr >> 8 & 0xff),
				(ip_addr >> 16 & 0xff), (ip_addr >> 24 & 0xff), ntohs(thanos_scan.scanner[i].port));
			pthread_mutex_unlock(&mutexsum);
			close(sockfd);
			continue;
		}


		recv_buffer = xmalloc(thanos_scan.scanner[i].max_len);

		timeout.tv_sec = thanos_scan.scanner[i].timeout;

		if(setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
			err("setsockopt failed\n");
		}

		if(thanos_scan.scanner[i].data_size){
			send(sockfd, thanos_scan.scanner[i].send_data, thanos_scan.scanner[i].data_size, 0);
		}

		recv_ret = recv(sockfd, recv_buffer, thanos_scan.scanner[i].max_len, MSG_WAITALL);

		pthread_mutex_lock(&mutexsum);
		good("%d.%d.%d.%d:%d open\n\n", (ip_addr & 0xff), (ip_addr >> 8 & 0xff),
				(ip_addr >> 16 & 0xff), (ip_addr >> 24 & 0xff), ntohs(thanos_scan.scanner[i].port));

		if(recv_ret > 0){
			if(hexdump){
				th4n0s_hex(recv_buffer, recv_ret);
			} else {
				for(j=0; j<recv_ret; j++){
					if(nowide && !isprint(recv_buffer[j])){
						putchar('.');
					} else {
						putchar(recv_buffer[j]);
					}
				}
			}
			printf("\n\n");
		}
		pthread_mutex_unlock(&mutexsum);

		xfree(recv_buffer);
		close(sockfd);
	}
	
	return NULL;
}


