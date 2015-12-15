#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#define MSECS 1000000
//int create_udp_socket(char* hostname, int port);

int opt = 0;
char* hn = NULL;
int port = 0;
int rate = 0;
int num = 0;
int seq = 0;
unsigned int length = 0;
int echo = 0;
long ratems = 0;
struct sock {
	struct sockaddr_in addr;
	int desc;};

int hostname_to_ip(char* hostname, char* ip) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL){
        herror("gethostbyname");
        return 1;
    }
    addr_list = (struct in_addr **)he->h_addr_list;

    for(int i = 0; addr_list[i] != NULL; i++) {
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }
    return 1;
}

void * echoer (void * arg) {
	struct sock *s = arg;
	for(int i = 0; i < length; i++){
	char buf[50009];
	int echo_addr = sizeof(s->addr);
	if (recvfrom(s->desc, buf, 50009, 0, (struct sockaddr *)&s->addr, &echo_addr) < 0) {
		perror("recv");
	}
	buf[50009] = '\0';
	int seqe; 
	memcpy(&seqe, buf+1, sizeof(int));

	printf("echo %d\nData:%hhx%hhx%hhx%hhx\n", ntohl(seqe), buf[9], buf[10], buf[11], buf[12]);
	}
}
int send_packet(int socket_desc, struct sockaddr_in serv_addr) {

	char data[length+9];//type-seq-len-data
	memset(data, 0xaa, sizeof(data));
	uint len = htonl(length);
	int sq;
	memcpy(data+5, &len, sizeof(int));
    printf("sending %d bytes to socket %d, %s:%d\n", sizeof(data), socket_desc,
            inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    for (int i = 0; i < num; i++) {
		if(i==num-1) data[0] = 'E';
		else data[0] = 'D';
		sq = htonl(seq);
		memcpy(data+1, &sq, sizeof(int));
        if (sendto(socket_desc, data, sizeof(data), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("sendto");
        return 1;
        }
        printf("%d\n",ntohl(sq));
        for (int j = 9; j < 14; j++) {
            printf("%hhx", data[j]);
        }
		puts("");
		seq+= length;

        usleep(ratems);
    }
    return 0;
}

int create_udp_socket() {
    int socket_desc;
    struct sockaddr_in serv_addr;

    if ((socket_desc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        puts("Your socket failed");
        exit(1);
    }
    
    char ip[100];

    hostname_to_ip(hn, ip);
    printf("%s resolved to %s\n" , hn , ip); 
    serv_addr.sin_family = AF_INET;
    inet_aton(ip, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);
	if(echo) { 
		struct sock s;
		s.addr = serv_addr;
		s.desc = socket_desc;
		pthread_t thrd;
		pthread_create(&thrd, NULL, echoer, &s);
	}

    send_packet(socket_desc, serv_addr);

    return 0;
}



int main (int argc, char *argv[]) {

    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                hn = argv[i];
                printf("hostname: %s\n", hn);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                port = atoi(argv[i]);
                if (port > 65535 || port < 1025) {
                    puts("Port must be between 1025 and 65535");
                    exit(1);
                }
                printf("port: %d\n", port);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                rate = atoi(argv[i]);
                printf("rate: %d\n", rate);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                num = atoi(argv[i]);
                printf("num: %d\n", num);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-q") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                seq = atoi(argv[i]);
                printf("seq_no: %d\n", seq);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-l") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                length = atoi(argv[i]);
                if (length < 1 || length > 50000) {
                    puts("Invalid length of packet");
                    exit(1);
                }
                printf("length: %d\n", length);
            } else { puts("what are you doing"); }
        } else if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                opt += 1;
                i++;
                echo = atoi(argv[i]);
                printf("echo: %d\n", echo);
                if (echo != 0 && echo != 1) {
                    puts("Echo must be 0 or 1");
                    exit(1);
                }
            } else { puts("what are you doing"); }
        }
    }
    if (opt != 7) {
        puts("Invalid number of arguments");
        exit(1);
    }

    ratems = (long)MSECS / (long)rate;
    create_udp_socket();

}

