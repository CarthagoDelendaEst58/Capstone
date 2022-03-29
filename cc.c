#include "defines.h"

int sensor_ids[MAXSENSORS];
int sensor_fovs[MAXSENSORS*2];
int active_sensors = 0;
int sensors_accepted = 0;

void handle_request(char * req, int connfd) {
    char resp[MAXBUF];
    int code = extract_code(req);
    if (code == DEFINESENSOR) {
        // sensors_accepted++;
        // sprintf(resp, "%d", PORT+(sensors_accepted%MAXSENSORS));
        // printf("%s\n", resp);
        // write(connfd, resp, MAXBUF);
        printf("Sensor Accepted");
    }
}

int main() {
    int sockfd, connfd, serverlen, clientlen;
    struct sockaddr_in serveraddress, clientaddress;
    char buf[MAXBUF];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error creating socket: %d\n", errno);
        exit(0);
    }

    bzero(&serveraddress, sizeof(serveraddress));
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_addr.s_addr = inet_addr(SERVERADDRESS);
    serveraddress.sin_port = htons(PORT);

    serverlen = sizeof(serverlen);
    if ((bind(sockfd, (struct sockaddr*)&serveraddress, sizeof(serveraddress)), &serverlen) < 0) {
        printf("error binding socket: %d\n", errno);
        exit(0);
    }

    if ((listen(sockfd, MAXSENSORS-1)) < 0) {
        printf("error listening: %d\n", errno);
        exit(0);
    }

    printf("listening on %s:%d\n", SERVERADDRESS, PORT);

    clientlen = sizeof(clientaddress);
    while (connfd = accept(sockfd, (struct sockaddr*)&clientaddress, &clientlen)) {
        printf("client accepted\n");

        read_request(connfd, buf);
        printf("%s\n", buf);
        handle_request(buf, connfd);
    }

    close(sockfd);
}