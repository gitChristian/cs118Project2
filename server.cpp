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
#include "segment.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    //check that a port number was provided
	if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     //create a UDP socket for server to use
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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

    // used to store client address info
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_length;

    //loop until file transmission is complete
    while(1){
       // recvfrom(sockfd, hello, 10, 0,(struct sockaddr*)&cli_addr, &cli_addr_length );
	    
    }
    close(sockfd);
}