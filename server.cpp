#include <iostream>
#include <cstdio>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
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
    fclose(file);
}

/*initializes file and total_segments */
void processRequest(){
  if(recvfrom(sockfd, &req_seg, sizeof(req_seg), 0, 
  (struct sockaddr*) &cli_addr, &cli_addr_length) > 0){
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
      printf("Total segments to be sent: %d\n", total_segments);
    }
    else
      fprintf(stderr,"ERROR, recieved wrong segment format.\n");
  }
  else
    fprintf(stderr,"ERROR, could not recieve.\n");
}

//send data using GBN
void sendData(){
  sendFirstWindow();
  while(base <= total_segments){
    
    //timeout
    if(timer + TIMEOUT < time(NULL)){
      printf("Timeout from segment number%d\n", base);
      //resend window
      for (list<struct segment>::iterator it=window_list.begin(); it != window_list.end(); it++){
        if(sendto(sockfd, &(*it), it->length + sizeof(int) *2 + sizeof(mode), 0,
        (struct sockaddr*)&cli_addr, cli_addr_length ) < 0 ){
          fprintf(stderr,"ERROR, couldn't send data.\n");
          exit(1);}
        printf("Resending packet number %d\n", it->seq_num );
      }
      time(&timer);
    }

    /*
   //TO BE DELETED
    if(recvfrom(sockfd, &req_seg, sizeof(req_seg), MSG_DONTWAIT, 
    (struct sockaddr*) &cli_addr, &cli_addr_length) < 0){
      fprintf(stderr,"ERROR, couldn't recieve ack.\n%s \n", strerror(errno));
      printf("%d\n", errno );
      exit(1);
    }
    */
  
    //recieved expected ACK number
    if(recvfrom(sockfd, &req_seg, sizeof(req_seg), MSG_DONTWAIT, 
    (struct sockaddr*) &cli_addr, &cli_addr_length) > 0){
     // printf("%d and type %d\n", req_seg.seq_num, req_seg.type );
      if(req_seg.type == ACK && req_seg.seq_num == base){
        printf("Recieved ACK for segment: %d\n", req_seg.seq_num);
        if(++base > total_segments)
          return;
        window_list.pop_front();
        if(next_seq_num <= total_segments){
          sendNextSegment();
          time(&timer);
        }
      }
    }

  }
}

void sendFirstWindow(){
  while(next_seq_num < base + WINDOW_SIZE && next_seq_num <= total_segments)
    sendNextSegment();
}

void sendNextSegment(){
  makeNextSegment();
  window_list.push_back(rspd_seg);
  if(sendto(sockfd, &rspd_seg, rspd_seg.length + sizeof(int) *2 + sizeof(mode), 0,
  (struct sockaddr*)&cli_addr, cli_addr_length ) < 0 ){
    fprintf(stderr,"ERROR, couldn't send data.\n%s \n", strerror(errno));
    printf("%d\n", errno );

    exit(1);}
  printf("Sending packet number %d\n", rspd_seg.seq_num );
  if(base == next_seq_num)
    time(&timer);
  if(next_seq_num <= total_segments)
    next_seq_num++;
}

void makeNextSegment(){
  rspd_seg.type = DATA;
  rspd_seg.seq_num = next_seq_num;
  rspd_seg.length = fread(rspd_seg.data, 1, SEG_DATA_SIZE, file);
}
