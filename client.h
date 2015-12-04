#include "segment.cpp"

double LOSS_PROB = 0;
double CORRUPT_PROB = 0.2;


int sockfd, portno, n, expected_seq_no = 1;
int serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
char *hostname, *filename;
struct segment rspd_seg, req_seg, corr_seg;
FILE* file;


void segmentControl();
void segmentCorrupted();
void segmentConfirm();