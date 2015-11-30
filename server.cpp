#include <iostream>
#include <cstdio>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <cstdlib>
#include <cstring>
#include <sys/wait.h> /* for the waitpid() system call */
#include <signal.h> /* signal name macros, and the kill() prototype */
#include <unistd.h>
#include <sys/stat.h>
#include "segment.cpp"

using namespace std;

/*various global variables */

// used to store client address info
struct sockaddr_in cli_addr;
socklen_t cli_addr_length;
//client's requested file
FILE* file;
//number of segments the file needs
int file_size;
struct segment rspd_seg, req_seg;
int sockfd;


void processRequest(){
  while(1){
    if(recvfrom(sockfd, &req_seg, sizeof(req_seg), 0, (struct sockaddr*) &cli_addr, &cli_addr_length) > 0){
      if(req_seg.type == REQ){
        puts("Got a request for a file.");
        file = fopen(req_seg.data, "r");
        if(file == NULL){
          fprintf(stderr,"ERROR, couldn't find file.\n");
          exit(1);
        }

        struct stat s;
        stat(req_seg.data, &s);
        file_size = s.st_size/MAX_SEGMENT_SIZE;
        if(s.st_size % MAX_SEGMENT_SIZE != 0)
          file_size++;
      }
      return;
    }
  }  
}


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

    //loop until file transmission is complete
    while(1){
       // recvfrom(sockfd, hello, 10, 0,(struct sockaddr*)&cli_addr, &cli_addr_length );
	    
    }
    close(sockfd);
}