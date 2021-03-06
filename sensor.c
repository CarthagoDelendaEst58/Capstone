#include "defines.h"

char sensor_id[ID_LEN];
int fov[2];
int port;

int status = INIT;

// Parses the command line options
// -i : Sensor ID
// -f : Sensor fov. Values are delimited by ','
// -p : Sensor port
void parse_argv(int argc, char* argv[]) {
    int arg_res = 0;
    int i = 0;
    while (i < argc) {
        if (!strcmp(argv[i], "-i") && i<(argc-1)) {
            strncpy(sensor_id, argv[i+1], ID_LEN);
            i += 2;
        }
        else if (!strcmp(argv[i], "-f") && i<(argc-1)) {
            fov[0] = atoi(strtok(argv[i+1], ","));
            fov[1] = atoi(strtok(NULL, ","));
            i += 2;
        }
        else if (!strcmp(argv[i], "-p") && i<(argc-1)) {
            port = atoi(argv[i+1]);
            i += 2;
        }
        else {
            i++;
        }
    }
}

// Announces to the server that this sensor exists
// Gives the server this sensor's ID and FOV
int announce_existence(int sockfd, struct sockaddr_in serveraddress) {
    char buf[MAXBUF];
    bzero(buf, MAXBUF);
    sprintf(buf, "%d sensid:%s,fov:{%d,%d}", DEFINESENSOR, sensor_id, fov[0], fov[1]);

    if (connect(sockfd, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0) {
        printf("error connecting: %d\n", errno);
        exit(0);
    }

    write(sockfd, buf, MAXBUF);
    
    close(sockfd);

    return 0;
}

// Sends the server the sensor's current status code
void respond_status(int connfd) {
    char resp[MAXBUF];
    sprintf(resp, "%d", status);
    write(connfd, resp, MAXBUF);
}

// Determines request type and responds accordingly
void handle_request(char* req, int connfd) {
    int code = extract_code(req);
    if (code == GETSTATUS) {
        respond_status(connfd);
    }
}

// Initialize sensor and begin communication with CC server
int main(int argc, char* argv[]) {
    int sockfd1, sockfd2, connfd, clientlen, serverlen;
    struct sockaddr_in serveraddress, clientaddress;
    char buf[MAXBUF];
    
    // Initialize
    bzero(sensor_id, ID_LEN);
    parse_argv(argc, argv);
    
    if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error creating socket: %d\n", errno);
        exit(0);
    }

    bzero(&serveraddress, sizeof(serveraddress));
    serveraddress.sin_family = AF_INET;
    serveraddress.sin_addr.s_addr = inet_addr(SERVERADDRESS);
    serveraddress.sin_port = htons(PORT);

    // Ping server
    announce_existence(sockfd1, serveraddress);

    // Initialize socket for listening
    if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("error creating socket: %d\n", errno);
        exit(0);
    }

    bzero(&clientaddress, sizeof(clientaddress));
    clientaddress.sin_family = AF_INET;
    clientaddress.sin_addr.s_addr = inet_addr(SERVERADDRESS);
    clientaddress.sin_port = htons(port);

    clientlen = sizeof(clientlen);
    if ((bind(sockfd2, (struct sockaddr*)&clientaddress, sizeof(clientaddress)), &clientlen) < 0) {
        printf("error binding socket: %d\n", errno);
        exit(0);
    }

    if ((listen(sockfd2, MAXSENSORS-1)) < 0) {
        printf("error listening: %d\n", errno);
        exit(0);
    }

    printf("listening on %s:%d\n", SERVERADDRESS, port);

    status = AVAIL;

    // Main communication loop
    serverlen = sizeof(serveraddress);
    while (connfd = accept(sockfd2, (struct sockaddr*)&serveraddress, &serverlen)) {
        printf("client accepted\n");
        read_request(connfd, buf);
        handle_request(buf, connfd);
    }
}