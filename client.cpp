#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <time.h>
#include "client.h"


double probability() {
    return (double) rand()/(double) RAND_MAX;
}

int main(int argc, char **argv) {
    char buf[BUF_SIZE];

    if (argc != 4) {
       fprintf(stderr,"Usage Error. \n");
       exit(1);     }

    /* create the socket for client to use*/
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        fprintf(stderr,"ERROR, couldn't get socket.\n");

    /* Get server DNS Entry*/
    hostname = argv[1];
    server = gethostbyname(hostname);

    if (server == NULL) {
        fprintf(stderr,"Error No Host. \n");
        exit(1);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);    
    portno = atoi(argv[2]);
    serveraddr.sin_port = htons(portno);

    /* build request */
    bzero((char *) &req_seg, sizeof(req_seg));
    filename = argv[3];
    strcpy(req_seg.data, filename);
    req_seg.length = strlen(filename);
    req_seg.type = REQ;

    /* send request to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, &req_seg, req_seg.length + sizeof(int) *2 + sizeof(mode), 
        0, (struct sockaddr*) &serveraddr, serverlen);
    if (n < 0){
        fprintf(stderr,"Error in requested file. \n");
        exit(1);        }

    rspd_seg.length = SEG_DATA_SIZE;
    expected_seq_no = 1;
    file = fopen(strcat(filename, "_copy"), "wb");

    bzero((char *) &req_seg, sizeof(req_seg));
    req_seg.length = sizeof(int) * 3;
    req_seg.type = ACK;
    req_seg.seq_num = expected_seq_no - 1;

    srand(time(NULL));
    while (1) {
        segmentControl();
    }
    fclose(file);
    return 0;
}


void segmentControl(){
    if (recvfrom(sockfd, &rspd_seg, sizeof(rspd_seg), 0, (struct sockaddr*) &serveraddr, (socklen_t*) &serverlen)
    < 0 || probability() < LOSS_PROB) {
        printf("segment lost!\n");
    }
    else
        segmentCorrupted();
}

void segmentCorrupted(){
    if (probability() < CORRUPT_PROB) {
        printf("segment corrupted!\n");
        if (sendto(sockfd, &req_seg, req_seg.length, 0, (struct sockaddr*) &serveraddr, serverlen) < 0){
            fprintf(stderr,"Error corrupted file. \n");
            exit(1);
        }
    }
    else
        segmentConfirm();
}

void segmentConfirm(){
    printf("Received segment number %d\n", rspd_seg.seq_num);
    fwrite(rspd_seg.data, 1, rspd_seg.length, file);
    req_seg.seq_num = rspd_seg.seq_num;
    if (sendto(sockfd, &req_seg, req_seg.length, 0, (struct sockaddr*) &serveraddr, serverlen) < 0){
        fprintf(stderr,"Error in Acking");
        exit(1);
    }
    printf("ACK'd segment %d\n", req_seg.seq_num);
    expected_seq_no++;
}