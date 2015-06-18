#include "thanos_str.h"

int th4n0s_inet_addr(const char *str, uint32_t *ret){
	int i = 0, pos = 0;
	unsigned char ipv4[4]={0};

	if(*str == '.'){
		return 0;
	}

	while(*str){
		if( pos == 4 || i == 4){
			return 0;
		}

		if(*str=='.'){
			i=0;
			pos++;
		}

		else if(*str < '0' || *str > '9'){
			return 0;
		}

		else {
			if(ipv4[pos] * 10 > 0xff){
				return 0;
			} else {
				ipv4[pos] *= 10;
			}

			if(ipv4[pos] + *str-'0' > 0xff){
				return 0;
			} else {
				ipv4[pos] += *str-'0';
			}

			i++;
		}

		str++;

	}

	if(!i){
		return 0;
	}

	*ret = (ipv4[0] << 24 | ipv4[1] << 16 | ipv4[2] << 8 | ipv4[3]);

	return 1;
}

void th4n0s_hex(const char *x, int size){

	int tot = 0, i = 0, aux = 0, pf = 0, count = 0;

	while(1){
		printf("\t0x%03d0: ",count);

		(tot+16 > size) ? (tot = size) : (tot += 16);

		for(i=aux;i<tot;i+=2){
			pf += printf(" %02x",(unsigned int)(*(unsigned char*)(&x[i])));

			if(i+1 != tot){
				pf += printf("%02x",(unsigned int)(*(unsigned char*)(&x[i+1])));
			}
		}

		printf("%-*s",42-pf,"");

		for(i=aux;i<tot;i++){
			printf("%c",(x[i] > 32 && x[i] < 127) ? x[i] : '.');
		}

		count++;

		printf("\n");
		pf = 0;

		if(i >= size)
			break;

		aux += 16;
	}

}

void hexencode(const char *input, char *output, size_t len){
        const char ht[]="0123456789abcdef"; // hextable

        size_t i = 0, j = 0;

        for(i=0; i<len; i++){
                output[j]=ht[(((unsigned char)input[i]/16)%16)];
                output[++j]=ht[(unsigned char)input[i]%16];
                j++;
        }

        output[j] = 0;

        return;
}

void hexdecode(const unsigned char *input, char *output){
	size_t i, j;

	for(i=0, j=0; input[i]; i+=2, j++){
		output[j] = (input[i]-'0')*16;
		output[j] += (input[i+1]-'0');
	}

	return;
}