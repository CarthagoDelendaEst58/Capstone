#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

// network specs
#define PORT 8000
#define SERVERADDRESS "127.0.0.1"

// data specs
#define ID_LEN 8
#define MAXSENSORS 10
#define MAXBUF 512

// status codes
#define INIT 0
#define AVAIL 1
#define BUSY 2
#define DEFINESENSOR 10
#define GETSTATUS 20

void read_request(int connfd, char* dest) {
    char buf[MAXBUF];
    bzero(buf, MAXBUF);

    read(connfd, buf, MAXBUF);

    memcpy(dest, buf, MAXBUF);
}

int extract_code(char* req) {
    char * tok = strtok(req, " ");
    return atoi(tok);
}