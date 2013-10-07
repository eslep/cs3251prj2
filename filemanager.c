#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"filemanager.h"


/*
 * FNV string hashing algorithm
 * 
 */
unsigned int fnv_hash (void* key, int len)
{
    unsigned char* p = key;
    unsigned int h = 2166136261;
    int i;

    for (i = 0; i < len; i++)
        h = (h*16777619) ^ p[i];

    return h;
}

char* filetypes[] = {"*.mp3","*.wav","*.ogg","*.flac","*.aac","*.wma"};
file_info file_table[FILETABLE_SIZE];
int numEntries;

//test code
/*int main()
{
	updateFiles();
	file_info* list2 = malloc(2*sizeof(file_info));
	strcpy(list2[0].filename,"header");
	list2[0].checksum[0] = (char)1;
	strcpy(list2[1].filename,"1.mp3");
	strcpy(list2[1].checksum,"d41d8cd98f00b204e9800998ecf8427e");
	file_info* list;
	getList(&list);
	int i=0;
	while(i<3)
	{
		if(i==0)
			printf("Filename: %s Checksum: %d\n",list[i].filename,list[i].checksum[0]);
		else
			printf("Filename: %s Checksum: %s\n",list[i].filename,list[i].checksum);
		i++;
	}
	file_info* diff;
	getDiff(&list2,&diff);
	int j=0;
	while(j<(int)(diff[0].checksum[0]+1))
	{
		if(j==0)
			printf("Filename: %s Checksum: %d\n",diff[j].filename,diff[j].checksum[0]);
		else
			printf("Filename: %s Checksum: %s\n",diff[j].filename,diff[j].checksum);
		j++;
	}
	free(diff);
	free(list);
}*/

void updateFiles()
{
	memset(file_table,0,sizeof(file_info)*FILETABLE_SIZE);
	numEntries = 0;
	char result[32];
	//open pipe to the command
	FILE *fd = popen("find . -maxdepth 1 -type f -name '*.mp3' -o -name '*.wav' -o -name '*.flac' -o -name '*.ogg' | cut -d '/' -f 2","r");
	int i = 0;
	while(fgets(result,31,fd) != NULL)
	{
		char *newline = strstr(result,"\n");
		*newline = '|';//replace newline with another character

		char *filename;
		filename = strtok(result,"|");//get the filename without the newline

		char hash_cmd[60];
		strcpy(hash_cmd,"md5sum ");//build command "md5sum [filename] | cut -d ' ' -f 1"
		strcat(hash_cmd,filename);
		strcat(hash_cmd," | cut -d ' ' -f 1");

		//compute the file checksum with md5sum
		FILE *hash_fd = popen(hash_cmd,"r");
		char checksum[34];
		while(fgets(checksum,33,hash_fd) != NULL)
		{
			if(*checksum!='\n')
			{
				file_info entry;
				strcpy(entry.filename,filename);
				strcpy(entry.checksum,checksum);
				int index = fnv_hash(entry.filename,strlen(entry.filename))%FILETABLE_SIZE;
				printf("Hash result: %d\n",index);
				file_table[index] = entry;
				numEntries++;
			}
		}
		i++;
		pclose(hash_fd);
	}
	pclose(fd);
}


/*
 * Function to get the file list
 * @param list Pointer to where the list should be stored
 */
void getList(file_info** list)
{
	*list = malloc(sizeof(file_info)*(numEntries+1));//malloc space for the file list and
	file_info header;
	strcpy(header.filename,"header");
	header.checksum[0] = (char)numEntries;
	*list[0] = header;

	int index=1;
	int j=0;
	while(j<FILETABLE_SIZE)
	{
		if(file_table[j].filename[0]!=0)
		{
			(*list)[index] = file_table[j];
			index++;
		}
		j++;
	}
}

void getDiff(file_info** compareTo, file_info** diff)
{
	file_info* header = &((*compareTo)[0]);
	int lenCompare = (int)((*header).checksum[0]);//get the number of elements in the list from the header
	file_info* list = &((*compareTo)[1]);//ignore the header from the beginning of our list
	*diff = malloc(sizeof(file_info)*(lenCompare+1));//malloc space for diff list and new header

	file_info newHeader;//prep header for diff list
	strcpy(newHeader.filename,"header");
	(*diff)[0] = newHeader;

	int i=0;
	int index=1;
	while(i<lenCompare)
	{
		int key = fnv_hash(list[i].filename,strlen(list[i].filename)) % FILETABLE_SIZE;
		if(strcmp(file_table[key].checksum,list[i].checksum)!=0)//if we can't find our list item's checksum in the table
		{	
			(*diff)[index] = list[i];//then add the list item to the diff list
			index++;
		}
		i++;
	}
	(*diff)[0].checksum[0] = (char)(index-1);
	free(header);//remove the header
}
