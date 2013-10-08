/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR: Kevin Chu
* PROJECT:	CS 3251 Project 1 - Professor Traynor
* DESCRIPTION:	Network Server Code
*
*////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h> //for socket(0, bind() and connect()
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

#define MAXPENDING 100 //Maximum outstanding connection
#define RCVBUFSIZE 512
#define SNDBUFSIZE 512
#define FILETABLE_SIZE 20000

//Structure of arguments to pass to client thread
typedef struct ThreadArgs
{
    int clntSock;
} ThreadArgs;

typedef struct
{
	char filename[32];
	char checksum[34];
} file_info;


void DieWithError(char* errorMessage); //Error handling function
int CreateTCPServerSocket(unsigned short port); //Create TCP server socket
int AcceptTCPConnection(int servSock); //accept tcp connection request
void *ThreadMain(void *arg);            //Structure of arguments to pass to client thread
int HandleTCPClient(int clntSocket);
void ReceiveFile(int clientSock, char* filename);
void getList(file_info** list, file_info** file_table, int numEntries);
int updateFiles(file_info** file_table);
unsigned int fnv_hash (void* key, int len);


int numClients=0;

int main(int argc, char *argv[])
{
    //printf("test");
    int servSock;                   //socket descriptor for server
    int clntSock;                   //Socket descriptor for client
    unsigned short echoServPort;    //Server port
    pthread_t threadID;             //Thread ID from pthread_create
    struct ThreadArgs *threadArgs;  //Pointer to argument structure for thread
    
    if(argc!=2)
    {
        fprintf(stderr,"Usage: %s <SERVER PORT>\n", argv[0]);
        exit(1);
    }
    
    
    echoServPort=atoi(argv[1]);     //First arg: local port
    
    servSock = CreateTCPServerSocket(echoServPort);
    
    for(;;) //Run forever
    {
        clntSock = AcceptTCPConnection(servSock);
        if((threadArgs=(struct ThreadArgs *) malloc(sizeof(struct ThreadArgs)))==NULL)
            DieWithError("malloc() failed");
        threadArgs -> clntSock =clntSock;
        
        //create client thread
        if(pthread_create(&threadID,NULL, ThreadMain,(void*) threadArgs)!=0)
            DieWithError("pthread_create() failed");
        
        printf("with thread %ld\n",(long int) threadID);
    }
}


//May not be necessary for threading
int CreateTCPServerSocket(unsigned short port)
{
    int sock;                       //Socket to create
    struct sockaddr_in echoServAddr; //local address
    
    //Create Socket for incoming connections
    if((sock=socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
        DieWithError("socket() failed");
    
    //Construct local address structure
    memset(&echoServAddr, 0, sizeof(echoServAddr)); //Zero out Structure
    echoServAddr.sin_family=AF_INET; //Internet address family
    echoServAddr.sin_addr.s_addr= htonl(INADDR_ANY); //Any incoming interface
    echoServAddr.sin_port = htons(port); //Local port
    
    //bind to local address
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) <0)
        DieWithError("bind() failed");
    
    //Mark the socket so it will isten for incoming connection
    if(listen(sock,MAXPENDING)<0)
        DieWithError("listen() failed");
    
    return sock;
    
}

int AcceptTCPConnection(int servSock)
{
    int clntSock;
    struct sockaddr_in echoClntAddr;
    unsigned int clntLen;
    
    //set the size of the in-out parameter
    clntLen=sizeof(echoClntAddr);
    if((clntSock=accept(servSock,(struct sockaddr *) &echoClntAddr,&clntLen))<0)
        DieWithError("accept() failed");
    //clntSOck is connected to a client!
    
    printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    return clntSock;
    
}






void *ThreadMain(void *threadArgs)
{
    int clntSock;       //socket descriptor for client connection
    
    //Guarantees that thread resources are deallocated upon return
    
    //extract socket file descriptor from argument
    clntSock = ((struct ThreadArgs *) threadArgs) -> clntSock;
//  free(threadArgs);      //deallocate memory for argument
    int go=1;
    while(go==1)
    {
    go=HandleTCPClient(clntSock);
    }
        free(threadArgs);      //deallocate memory for argument

    return (NULL);
}

/*
 * Process commands sent to the server
 *
 */
void HandleCommand(char* cmd)
{

}
int HandleTCPClient(int clntSocket)
{
    //printf("handling client");
    char echoBuffer[RCVBUFSIZE];   //Buffer for echo string
    memset(echoBuffer,0,RCVBUFSIZE);
    int recvMsgSize;
    
    //recieve message from client
    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0))<0)
        DieWithError("recv() failed");


    
    // Send received string and receive again until end of transmission
    if(recvMsgSize>0)
    {
        //Echo message back to client
            if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
                DieWithError("send() failed");
        
            // See if there is more data to receive
           /* if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) <0)
                DieWithError("recv() failed");*/
    }
            printf("Command:%s\n", echoBuffer);

    /*int size= (sizeof(echoBuffer)/sizeof(echoBuffer[0]));
    printf("Size: %u", size);*/
            printf("before");

    if(echoBuffer[0]=='s')
    {
        printf("hell yeah\n");
        ReceiveFile(clntSocket, "server/receive.m4a");
    }
    if(echoBuffer[0]=='q'||recvMsgSize==0)
    {
        printf("close:\n");
        close(clntSocket);
        return 0;
    }
    else if(echoBuffer[0]=='l')
    {
        //getList
        printf("one");
        file_info * file_table;
        int entries=updateFiles(&file_table);
        file_info* list;
        getList(&list,&file_table,entries);
        printf("one");
        file_info * header = (&list)[0];
        int numElements= header->checksum[0];
        printf("NumElements: %u",numElements);
        //send size of list packet;
        //while(count<size of list)
            //send filename length packet
            // send filename
    
        free(header);
        free(file_table);
    
    }
            printf("after");

    return 1;

}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void ReceiveFile(int clientSock, char* filename)
{
     /*int numberOfReceivedBytes=0;
    while (numberOfReceivedBytes<fileSize){
        numberOfReceivedBytes+=recv(newSocket,buffer + numberOfReceivedBytes,sizeof(*buffer),0)
    }*/
    
    printf("Recieve\n");
    //Recieve Size Packet
    /*long int sizeBuff[1];
    memset(sizeBuff,0,sizeof(long int));*/
    uint32_t un=0;
    if(recv(clientSock,&un,sizeof(uint32_t),0)<1)
    {
        DieWithError("Didn't get the long\n");
    }
    else
    {
        //sizeBuff[1]=ntohl(sizeBuff[1]);
        //printf("Length: %ld\n",sizeBuff[1]);
        un=ntohl(un);
        printf("Length: %u\n",un);
        
    }
    
    //Prepare to recieve data.
    char recvBuff[RCVBUFSIZE];   //Buffer for echo string
    memset(recvBuff,0,RCVBUFSIZE);
		FILE *filestream = fopen(filename, "a");
		if(filestream == NULL)
        {
			DieWithError("Cannot open new file on server.\n");
        }
		else
		{
            printf("Recieve2\n");

			memset(recvBuff,0, RCVBUFSIZE);
			int fileChunk = 0;
            int temp=0;
            int total=0;
            printf("Recieve3\n");
			//while((fileChunk = recv(clientSock, recvBuff, RCVBUFSIZE, 0)) > 0)
            while((fileChunk = recv(clientSock, recvBuff, RCVBUFSIZE, 0)) > 0 && (total+=fileChunk)<un)
            {
                printf("recieve: %u\n",temp);
                temp=temp+1;
                int i=0;
                
			    int write_sz = fwrite(recvBuff, sizeof(char), fileChunk, filestream);
  
                   // printf("Chunk: %s\n",recvBuff);
                
				if(write_sz < fileChunk)
			    {
			        printf("File write failed.\n");
                    break;
        
                    memset(recvBuff,0, RCVBUFSIZE);
                   /* if (fileChunk == 0 || fileChunk != 512)
                    {
                        break;
                    }*/
                }
            }
            printf("recieve: %u\n",temp);
            temp=temp+1;
            int write_sz = fwrite(recvBuff, sizeof(char), fileChunk, filestream);
            
            // printf("Chunk: %s\n",recvBuff);
            
            if(write_sz < fileChunk)
            {
                printf("File write failed.\n");
            }
			if(fileChunk < 0)
		    {

	                DieWithError("recv() failed");
        	}
			printf("Ok received from client!\n");
			fclose(filestream);
        }
}



void sendFile(int clientSock, char* filename)//take in a client socket
{
    
    /* Send the string to the server */
    /*	    FILL IN	 */
   
    /**int numberOfSentBytes=0;
    while (numberOfSentBytes<fileSize){
        numberOfSentBytes+=send(clientSock,buffer+numberOfSentBytes,fileSize-numberOfSentBytes+1,0);
    }*/
    
    //Find size of file
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    printf("seek\n");
    long int lengthOfFile = ftell(fp);
    fclose(fp);
    
    //send size packet
    /*long int sizeBuff[1];
    memset(sizeBuff,0,sizeof(long int));
    printf("Length: %ld\n",sizeBuff[1]);
    sizeBuff[1]=lengthOfFile;
    printf("Length: %ld\n",sizeBuff[1]);
    sizeBuff[1]=htonl(sizeBuff[1]);
    printf("Length: %ld\n",sizeBuff[1]);

    send(clientSock,sizeBuff,sizeof(long int),0);*/
    uint32_t un = htonl(lengthOfFile);
    send(clientSock, &un, sizeof(uint32_t), 0);
            printf("Length: %u\n",un);

    //Prepare to send data
    unsigned char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    memset(sndBuf, 0, RCVBUFSIZE);
    printf("Sending %s to the server... ", filename);
    FILE *filestream = fopen(filename, "r");
    if(filestream == NULL)
    {
        DieWithError("ERROR: File");
    }

		memset(sndBuf,0, SNDBUFSIZE);
		int fileChunk;
        int totalSize=0;
        int temp=0;
		while((fileChunk = fread(sndBuf, sizeof(char), SNDBUFSIZE, filestream)) > 0)
		{
            printf("sent: %u\n",temp);
           // printf("Chunk: %s\n",sndBuf);

            temp=temp+1;
            totalSize+=fileChunk;
            int count=0;
            
            int t=send(clientSock, sndBuf, fileChunk, 0);
            count+=t;
            int i=0;
          
		    while(count < fileChunk && t>0)
		    {
                 t=send(clientSock, sndBuf, fileChunk, 0);
		    }
            
            
		memset(sndBuf,0, SNDBUFSIZE);
		}
		printf("Ok file %s sent from client!\n", filename);
        		memset(sndBuf,0, SNDBUFSIZE);

	//}
   
    
}

/*
 * Function to get the file list from the file hash table
 * @param list Pointer to where the list should be stored
 * @param file_table Pointer to the file hash table to list
 */
void getList(file_info** list, file_info** file_table, int numEntries)
{
	*list = malloc(sizeof(file_info)*(numEntries+1));//malloc space for the file list and header entry
	memset(*list,0,sizeof(file_info)*(numEntries+1));
	strcpy((*list)[0].filename,"header");
	(*list)[0].checksum[0] = (char)numEntries;


	int index=1;
	int j=0;
	while(j<FILETABLE_SIZE)
	{
		if((*file_table)[j].filename[0]!=0)
		{
			(*list)[index] = (*file_table)[j];
			index++;
		}
		j++;
	}
}


int updateFiles(file_info** file_table)
{
	memset((*file_table),0,sizeof(file_info)*FILETABLE_SIZE);
	int numEntries = 0;
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
				(*file_table)[index] = entry;
				numEntries++;
			}
		}
		i++;
		pclose(hash_fd);
	}
	pclose(fd);
	return numEntries;
}

unsigned int fnv_hash (void* key, int len)
{
    unsigned char* p = key;
    unsigned int h = 2166136261;
    int i;

    for (i = 0; i < len; i++)
        h = (h*16777619) ^ p[i];

    return h;
}
