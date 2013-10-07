#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"filemanager.h"

int main()
{
	file_info* file_table = malloc(sizeof(file_info)*FILETABLE_SIZE);
	int numEntries = updateFiles((file_info**)(&file_table));
	file_info* list2 = malloc(2*sizeof(file_info));
	strcpy(list2[0].filename,"header");
	list2[0].checksum[0] = (char)1;
	strcpy(list2[1].filename,"1.mp3");
	strcpy(list2[1].checksum,"d41d8cd98f00b204e9800998ecf8427e");
	file_info* list;
	getList(&list, (file_info**)(&file_table), numEntries);
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
	getDiff(&list2,(file_info**)(&file_table),&diff);
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
	free(file_table);
}
