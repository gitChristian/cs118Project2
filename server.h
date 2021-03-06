#include <list>
#include "segment.cpp"

//WINDOW_SIZE is number of segments (not bytes)
#define WINDOW_SIZE 10
#define TIMEOUT 2

// used to store client address info
struct sockaddr_in cli_addr;
socklen_t cli_addr_length = sizeof(sockaddr_in);
//client's requested file
FILE* file;
//number of segments the file needs
int total_segments;
struct segment rspd_seg, req_seg;
int sockfd;
std::list<struct segment> window_list;
int base = 1, next_seq_num = 1;
time_t timer = 0;

void processRequest();
void makeNextSegment();
void sendFirstWindow();
void sendData();
void sendNextSegment();