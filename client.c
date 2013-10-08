/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Kevin Chu* PROJECT:	CS 3251 Project 1 - Professor Traynor
* DESCRIPTION:	Network Client Code
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>	    /* for OpenSSL EVP digest libraries/SHA256 */
#include <sys/stat.h>
#include <stdint.h>
#include "filemanager.h"

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define MDLEN 32
void sendCommand(int clientSock, unsigned char* command);// take in a socket and a command
void sendFile(int clientSock, char* filename);
int createSocket(char* addr);
struct stat st;


void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/* The main function */
int main(int argc, char *argv[])
{
    char * address="127.0.0.1";
    unsigned char flag ='a';
    int clientSock;


    if(argc==2)
    {
        address=argv[1];
    }
    else if (argc != 1)
    {
	printf("Incorrect input format. The correct format is:\n\tnameChanger your_name\n");
	exit(1);
    }
    clientSock = createSocket(address);

    //body
    while(flag!='q')
    {
    if(flag!='\n')
        printf("Please enter a command: ");
        
    flag = fgetc(stdin);
    //printf("%s",&flag);
    
    if(flag=='d'||flag=='p'||flag=='l')
    {
        /*if(flag=='l')
            executeLS(flag);*/
	char recvBuf[RCVBUFSIZE];
	switch(flag)
	{
		case 'l':
			sendCommand(clientSock,&flag);
			int bytesRecv = recv(clientSock, recvBuf, RCVBUFSIZE, 0);
			printf("recvBuf: ");
			int j=0;
   			while(j<RCVBUFSIZE)
    			{
				printf("%x",recvBuf[j]);
				j++;
    			}
   			 printf("\n");

			serial_file_info* serial_header = malloc(sizeof(serial_file_info));
			memset(serial_header,0,sizeof(serial_file_info));
			memcpy(serial_header,recvBuf,sizeof(serial_file_info));

			file_info* header = deserialize_info(serial_header);
			//free((*serial_header).buf);
			free(serial_header);
			//printf("Header: Filename: %s Length: %d\n",(*header).filename,(*header).checksum[0]);
	}
        
    }
    else if(flag=='s')
    {
        sendCommand(clientSock,&flag);
        //sendFile(clientSock,"test");
        sendFile(clientSock,"02 Everything Is Alright.m4a");

    }
    else if(flag=='\n'||flag=='q')
    {
    }
    else if(flag=='h')
    {
        printf("Commands: \n d:Diff\np:Pull\nl:List\n");
    }
    else
    {
        printf("Invalid Command: Enter h for help\n");
    }
   }
    char* q = "q";
    sendCommand(clientSock,q);

   close(clientSock);
}

int createSocket(char* addr)
{
 int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */

    char *studentName;		    /* Your Name */
    char *message;               /*String being sent*/

   
    /* Create a new TCP socket*/
    /*	    FILL IN	*/
    
    if((clientSock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0)
    {
        printf("Socket() print");
        DieWithError("Socket() Failed");
    }

    /* Construct the server address structure */
    /*	    FILL IN	 */
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(addr);  //What is the server IP?
    int servPort=50000;
    serv_addr.sin_port =htons(servPort);       //What is the serverPort?

    /* Establish connection to the server */
    /*	    FILL IN	 */
    if(connect(clientSock,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0)
        DieWithError("connect() Failed");
    
    return clientSock;
}


void sendCommand(int clientSock, unsigned char* command)// take in a socket and a command
{
    
    unsigned char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    
    int i;			    /* Counter Value */
    
    memset(sndBuf, 0, RCVBUFSIZE);
     /* Send the string to the server */
    /*	    FILL IN	 */
    unsigned int StringLen=strlen(command);//strlen(message);
   // printf("\nLength: %u\n",flag);
    unsigned int sentBytes=send(clientSock,command,StringLen,0);
    //printf ("sentBytes: %u", sentBytes);
    if(sentBytes!=StringLen)
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



