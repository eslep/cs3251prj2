typedef struct
{
	char filename[32];
	char checksum[34];
} file_info;

#define FILETABLE_SIZE 1000


void updateFiles();//update the local file table
void getList(file_info** list);//get a list of the files in the local file table
void getDiff(file_info** compareTo, file_info** diff);//get a list of the files in compareTo that aren't in the file table
