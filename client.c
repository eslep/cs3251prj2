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

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define MDLEN 32
void sendCommand(char *addr, unsigned char* command);
void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

/* The main function */
int main(int argc, char *argv[])
{
     if (argc != 1)
    {
	printf("Incorrect input format. The correct format is:\n\tnameChanger your_name\n");
	exit(1);
    }
    
    unsigned char flag ='a';
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
        sendCommand("127.0.0.1",&flag);
    }
    else if(flag=='s')
    {
        sendCommand("127.0.0.1",&flag);
    }
    else if(flag=='\n')
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
}

void sendCommand(char *addr, unsigned char* command)
{
    printf("%s",command);
    
    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* The server address */

    char *studentName;		    /* Your Name */
    char *message;               /*String being sent*/

    unsigned char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    unsigned char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    int i;			    /* Counter Value */
    
     memset(&sndBuf, 0, RCVBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

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
    //handle client
    
    /* Send the string to the server */
    /*	    FILL IN	 */
    unsigned int StringLen=strlen(command);//strlen(message);
    printf("\nLength: %u\n",StringLen);
    unsigned int sentBytes=send(clientSock,command,StringLen,0);
    printf ("sentBytes: %u", sentBytes);
    if(sentBytes!=StringLen)
        DieWithError("send() sent a different number of bytes than expected");

    /* Receive and print response from the server */
    /*	    FILL IN	 */
    int totalBytesRcvd=0;
    int bytesRcvd=0;
   // printf("Received: ");
        while(totalBytesRcvd<StringLen)
        {
            if((bytesRcvd=recv(clientSock,rcvBuf,RCVBUFSIZE-1,0))<=0)
                DieWithError("recv() failed or connection closed prematurely");
            totalBytesRcvd+=bytesRcvd; //Tally total bytes
            rcvBuf[bytesRcvd]='\0';
           
            
        }
    //printf(rcvBuf);
    printf("buff\n");
    printf("%s\n", rcvBuf);
   // printf("input is: ");
    //for(i = 0; i < MDLEN; i++) printf( rcvBuf[i]);
    printf("\n");
    close(clientSock);
    
}

