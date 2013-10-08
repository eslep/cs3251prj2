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
#include "../filemanager.h"
#include <stdint.h>

#define MAXPENDING 5 //Maximum outstanding connection


void DieWithError(char*errorMessage); //Error handling function
int CreateTCPServerSocket(unsigned short port); //Create TCP server socket
int AcceptTCPConnection(int servSock); //accept tcp connection request
void *ThreadMain(void *arg);            //Structure of arguments to pass to client thread
int HandleTCPClient(int clntSocket);
void ReceiveFile(int clientSock, char* filename);
void sendCharStream(int clientSock, char* stream, int length);

//Structure of arguments to pass to client thread
typedef struct ThreadArgs
{
    int clntSock;
} ThreadArgs;


connection client_list[15];
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
	connection new_client;
	new_client.clientSock = clntSock;
        if((threadArgs=(struct ThreadArgs *) malloc(sizeof(struct ThreadArgs)))==NULL)
            DieWithError("malloc() failed");
        threadArgs -> clntSock =new_client.clientSock;
        
        //create client thread
        if(pthread_create(&(new_client.threadID),NULL, ThreadMain,(void*) threadArgs)!=0)
            DieWithError("pthread_create() failed");
        client_list[numClients] = new_client;
	numClients++;
        printf("with thread %ld\n",(long int) new_client.threadID);
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
    free(threadArgs);      //deallocate memory for argument
    int go=1;
    while(go==1)
    {
    go=HandleTCPClient(clntSock);
    }
        //free(threadArgs);      //deallocate memory for argument

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
    memset(&echoBuffer,0,RCVBUFSIZE);
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
    if(recvMsgSize==0)
	return 0;
            //printf("Command:%s\n", echoBuffer);

    /*int size= (sizeof(echoBuffer)/sizeof(echoBuffer[0]));
    printf("Size: %u", size);*/
    if(echoBuffer[0]=='s')
    {
        printf("hell yeah\n");
        ReceiveFile(clntSocket, "server/receive.m4a");
    }
    if(echoBuffer[0]=='q')
    {
        printf("close:\n");
        close(clntSocket);
        return 0;
    }
    if(echoBuffer[0]=='l')
    {
	file_info* file_list;
        server_list(&file_list);
	file_info* header = &(file_list[0]);
	int list_len = (*header).checksum[0];

	serial_file_info* serial_header = serialize_info(header);
	sendCharStream(clntSocket, (char*)(*serial_header).buf, (*serial_header).length);
	
	/*file_info* remaining = filelist[1];
	int i=0;
	while(i<list_len)
	{
		serial_file_info* serialized = serialize_info(remaining[i]);
		message_len = (*serialized).length;
		sendCharStream(clntSocket, (*serialized).buf, message_len);
	}*/
	free(header);
	free(serial_header);
    }
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
    memset(&sizeBuff,0,sizeof(long int));*/
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
    memset(&recvBuff,0,RCVBUFSIZE);
		FILE *filestream = fopen(filename, "a");
		if(filestream == NULL)
        {
			DieWithError("Cannot open new file on server.\n");
        }
		else
		{
            printf("Recieve2\n");

			memset(&recvBuff,0, RCVBUFSIZE);
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
        
                    memset(&recvBuff,0, RCVBUFSIZE);
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

void sendCharStream(int clientSock, char* stream, int length)// take in a socket and a stream to send
{
    
    unsigned char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    
    int i;			    /* Counter Value */
    
    memset(sndBuf, 0, RCVBUFSIZE);
    memcpy(sndBuf, stream, length);
     /* Send the string to the server */
   // printf("\nLength: %u\n",flag);
    unsigned int sentBytes=send(clientSock,sndBuf,SNDBUFSIZE,0);
    int j=0;
    printf("sndBuf: ");
    while(j<SNDBUFSIZE)
    {
	printf("%x",sndBuf[j]);
	j++;
    }
    printf("\n");
    //printf ("sentBytes: %u", sentBytes);
    if(sentBytes!=length)
        DieWithError("send() sent a different number of bytes than expected");
    
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
    memset(&sizeBuff,0,sizeof(long int));
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
    memset(&sndBuf, 0, RCVBUFSIZE);
    printf("Sending %s to the server... ", filename);
    FILE *filestream = fopen(filename, "r");
    if(filestream == NULL)
    {
        DieWithError("ERROR: File");
    }

		memset(&sndBuf,0, SNDBUFSIZE);
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

