#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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
    char buf[512];
    int socket_desc;

    if ((socket_desc = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        puts("Your socket failed, kill yourself");
        exit(1);
    }

    memset((char*)&client, 0, sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(socket_desc, (struct sockaddr*)&client, sizeof(client)) == -1) {
        perror("bind");
        exit(1);
    }
    while (1) {
        if (recvfrom(socket_desc, buf, 512, 0, (struct sockaddr*)&serv, &slen) > 0) {
            printf("Received packet from %s:%d\nData: %x\n\n",
                inet_ntoa(serv.sin_addr), ntohs(serv.sin_port), buf);
            if (echo == 1){
                if ((sendto(socket_desc, buf, 512, 0, (struct sockaddr*)&serv, slen)) < 0) {
                perror("sendto");
                return 1;
                }   
            puts("echoing");
            }   
        } else { perror("recv"); }
    }
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

