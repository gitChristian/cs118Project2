/* The segment structure is what we use to
warp our data in order to implement reliable
data transfer.

length is number of bytes of valid data in 
int data

type represents the type of packet
data is empty if type = ACK

seq_num = sequence number
*/

#define MAX_SEGMENT_SIZE 1000

enum mode{ACK, REQ, DATA};

struct segment
{
	int seq_num;
	int length;
	mode type;
	int data[MAX_SEGMENT_SIZE - sizeof(int)*2 - sizeof(mode)];
};