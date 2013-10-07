#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"types.h"

typedef struct
{
	char filename[32];
	char checksum[34];
} file_info;

#define FILETABLE_SIZE 1000

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

void updateFiles();
void getList(file_info** list);
void getDiff(file_info** compareTo, file_info** diff);

char* filetypes[] = {"*.mp3","*.wav","*.ogg","*.flac","*.aac","*.wma"};
file_info file_table[FILETABLE_SIZE];
int numEntries;

int main()
{
	updateFiles();
	file_info* list2 = malloc(2*sizeof(file_info));
	strcpy(list2[0].filename,"header");
	list2[0].checksum[0] = (char)1;
	strcpy(list2[1].filename,"04-TwentyninePalms.mp3");
	strcpy(list2[1].checksum,"f2a9f141647ca3fa2783b4b079e3ad91");
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
}

void updateFiles()
{
	memset(file_table,0,sizeof(file_info)*FILETABLE_SIZE);
	numEntries = 0;
	char result[32];
	FILE *fd = popen("find . -maxdepth 1 -type f -name '*.mp3' -o -name '*.wav' -o -name '*.flac' -o -name '*.ogg' | cut -d '/' -f 2","r");
	int i = 0;
	while(fgets(result,31,fd) != NULL)
	{
		char *newline = strstr(result,"\n");
		*newline = '|';
		char *filename;
		filename = strtok(result,"|");

		char hash_cmd[60];
		strcpy(hash_cmd,"md5sum ");
		strcat(hash_cmd,filename);
		strcat(hash_cmd," | cut -d ' ' -f 1");

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
	*list = malloc(sizeof(file_info)*(numEntries+1));
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
	file_info* list = &((*compareTo)[1]);
	*diff = malloc(sizeof(file_info)*(lenCompare+1));
	printf("lenCompare: %d\n",lenCompare);

	file_info newHeader;//prep header for diff list
	strcpy(newHeader.filename,"header");
	(*diff)[0] = newHeader;

	int i=0;
	int index=1;
	while(i<FILETABLE_SIZE)
	{
		if(file_table[i].filename[0]!=0)
		{	
			printf("file_table i=%d filename=%s checksum=%s\n",i,file_table[i].filename,file_table[i].checksum);	
			int j=0;
			int found=0;
			while(!found && j<lenCompare)
			{
				printf("list j=%d filename=%s checksum=%s\n",j,list[j].filename,list[j].checksum);
				if(strcmp(file_table[i].checksum,list[j].checksum)==0)
				{
					found = 1;
					printf("Found\n");
					break;
				}
				j++;
			}
			if(!found && index<(lenCompare+1))
			{
				
				(*diff)[index] = list[j];
				index++;
			}
			printf("\n\n");
		}
		i++;
	}
	(*diff)[0].checksum[0] = (char)(index-1);
	free(header);//remove the header
}
