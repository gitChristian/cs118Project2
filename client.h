#include "segment.cpp"


const int BUF_SIZE = 1024;
double LOSS_PROB = 0.00;
double CORRUPT_PROB = 0.0;


int sockfd, portno, n, expected_seq_no = 1;
int serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
char *hostname, *filename;
struct segment rspd_seg, req_seg;
FILE* file;


void segmentControl();
void segmentCorrupted();
void segmentConfirm();