#include"types.h"
#define FILETABLE_SIZE 1000

serial_file_info* serialize_info(file_info* data);//serialize a file_info struct
file_info* deserialize_info(serial_file_info* data);
void server_list(file_info** result);//run filemanager ops for list commands on the server
void client_diff(file_info** list, file_info** result);//run filemanager ops for diff commands on the client
int updateFiles(file_info** file_table);//update the local file table
void getList(file_info** list, file_info** file_table, int numEntries);//get a list of the files in the local file table
void getDiff(file_info** compareTo, file_info** file_table, file_info** diff);//get a list of the files in compareTo that aren't in the file table
