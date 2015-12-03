#include "segment.cpp"

int BUF_SIZE = 1024;
double LOSS_PROB = 0.00;
double CORRUPT_PROB = 0.2;

int sockfd, portno, n, expected_seq_no;
int serverlen;
struct sockaddr_in serveraddr;
struct hostent *server;
char *hostname, *filename;
char buf[BUF_SIZE];
FILE* file;

void segmentControl();
void segmentCorrupted();
void segmentConfirm();
