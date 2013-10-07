#define SNDBUFSIZE 512
#define RCVBUFSIZE 512

typedef struct
{
	char filename[32];
	char checksum[34];
} file_info;

typedef struct
{
	char flag;
	file_info payload;
} info_packet;

typedef struct connection
{
	int clientSock;
	pthread_t threadID;
	pthread_t file_managerID;
	char recv_buf[RCVBUFSIZE];
	char send_buf[SNDBUFSIZE];
} connection;
