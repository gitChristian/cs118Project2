/* The segment structure is what we use to
warp our data in order to implement reliable
data transfer.

length is number of bytes of valid data in 
int data

type represents the type of packet
data is empty if type = ACK
CLOSE is used by server to close connection

seq_num = sequence number
*/

#define MAX_SEGMENT_SIZE 1000

enum mode{ACK, REQ, DATA, CLOSE, CORRUPT};
const int SEG_DATA_SIZE = MAX_SEGMENT_SIZE - sizeof(int)*2 - sizeof(mode);

struct segment
{
	int seq_num;
	int length;
	mode type;
	char data[SEG_DATA_SIZE];
};