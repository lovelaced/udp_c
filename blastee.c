#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
//#include <sys/uio.h>

//int create_udp_socket(char* hostname, int port);

int opt = 0;
int port = 0;
int echo = 0;

int create_client_socket() {
    struct sockaddr_in client, serv;
    int slen=sizeof(serv);
    char buf[50009];
    int socket_desc;
	struct timeval tv;
	struct timeval start;
	struct tm* t;
	int total_packets =0; int total_bytes =0;
	gettimeofday(&start, NULL);
    if ((socket_desc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        puts("Your socket failed, kill yourself");
        exit(1);
    }
	struct timeval to;
	to.tv_sec = 5;
	setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
    memset((char*)&client, 0, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(socket_desc, (struct sockaddr*)&client, sizeof(client)) == -1) {
        perror("bind");
        exit(1);
    }
	int seq; unsigned int len;
	char type = 'D';
    while (type!='E') {
        if (recvfrom(socket_desc, buf, 50009, 0, (struct sockaddr*)&serv, &slen) > 0) {
			type = buf[0];
			memcpy(&seq, buf+1, sizeof(int));
			memcpy(&len, buf+5, sizeof(unsigned int));
			seq = htonl(seq);
			len = htonl(len);
			gettimeofday(&tv, NULL);
			t = localtime(&tv.tv_sec);
            printf("Received packet from %s:%d\n at %d:%d:%d.%d Type: %c, seq: %d, len: %u\n Data: %hhx%hhx%hhx%hhx\n",
                inet_ntoa(serv.sin_addr), ntohs(serv.sin_port),
				t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec / 1000,
				type, seq, len,
				buf[9],buf[10],buf[11],buf[12]);
			total_packets++;
			total_bytes += len;
            if (echo == 1){
				buf[0]='C';
                if ((sendto(socket_desc, buf, 50009, 0, (struct sockaddr*)&serv, slen)) < 0) {
                perror("sendto");
                return 1;
                }   
				puts("echoing");
            }   
        } else { perror("recv"); break;}
    }
	float duration; 
	struct timeval end;
	gettimeofday(&end, NULL);
	duration = (end.tv_sec - start.tv_sec);
	duration +=(end.tv_usec - start.tv_usec)/1000000;
	printf("Total packets: %d, Total bytes: %d, avg. packets/s %f, avg. bytes/s %f, duration %f\n",total_packets, total_bytes, total_packets/duration, total_bytes/duration, duration);
    close(socket_desc);
    return 0;
}


int main (int argc, char *argv[]) {

    int i;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) {
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
    if (opt != 2) {
        puts("Invalid number of arguments");
        exit(1);
    }
    create_client_socket();

}

