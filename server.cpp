#include <iostream>
#include <cstdio>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "server.h"

using namespace std;

int main(int argc, char *argv[])
{
  //check that a port number was provided
	if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     //create a UDP socket for server to use
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        fprintf(stderr,"ERROR, couldn't get socket.\n");

    //bind address of server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //ip address
    int portno = atoi(argv[1]);
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) {
              	fprintf(stderr,"ERROR, couldn't bind.\n");
         		    exit(1);
              }

    processRequest();
    sendData();  
    
    close(sockfd);
}

/*initializes file and total_segments */
void processRequest(){
  while(1){
    if(recvfrom(sockfd, &req_seg, sizeof(req_seg), 0, (struct sockaddr*) &cli_addr, &cli_addr_length) > 0){
      if(req_seg.type == REQ){
        puts("Got a request for a file.");
        file = fopen(req_seg.data, "rb");
        if(file == NULL){
          fprintf(stderr,"ERROR, couldn't find file.\n");
          exit(1);
        }

        struct stat s;
        stat(req_seg.data, &s);
        total_segments = s.st_size/MAX_SEGMENT_SIZE;
        if(s.st_size % MAX_SEGMENT_SIZE != 0)
          total_segments++;
      }
      return;
    }
  }  
}

//send data using GBN
void sendData(){
  sendFirstWindow();
  while(base <= total_segments){
    if(timer + TIMEOUT < time(NULL)){
      printf("Timeout from packet number%d\n", base);


      //.....
    }
  }

}

void sendFirstWindow(){
  while(next_seq_num < base + WINDOW_SIZE ){
    makeSegment(next_seq_num);
    window_list.push_back(rspd_seg);
    if(sendto(sockfd, &rspd_seg, rspd_seg.length + sizeof(int) *2 + sizeof(mode), 0,
      (struct sockaddr*)&cli_addr, cli_addr_length ) < 0 ){
          fprintf(stderr,"ERROR, couldn't send data.\n");
          exit(1);}
    printf("Sending packet number %d\n", rspd_seg.seq_num );
    if(base == next_seq_num)
      time(&timer);
    next_seq_num++;
  }
}

void makeSegment(int next_seq_num){
  rspd_seg.type = DATA;
  rspd_seg.seq_num = next_seq_num;
  rspd_seg.length = fread(rspd_seg.data, 1, SEG_DATA_SIZE, file);
}