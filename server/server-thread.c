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
#define MAXPENDING 5 //Maximum outstanding connection
#define RCVBUFSIZE 512

void DieWithError(char*errorMessage); //Error handling function
void HandleTCPClient(int clntSocket); //TCP client handling function
int CreateTCPServerSocket(unsigned short port); //Create TCP server socket
int AcceptTCPConnection(int servSock); //accept tcp connection request
void *ThreadMain(void *arg);            //Structure of arguments to pass to client thread
void HandleTCPClient(int clntSocket);

//Structure of arguments to pass to client thread
struct ThreadArgs
{
    int clntSock;
};


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
    free(threadArgs);      //deallocate memory for argument
    
    HandleTCPClient(clntSock);
    
    return (NULL);
}

void HandleTCPClient(int clntSocket)
{
    //printf("handling client");
    char echoBuffer[RCVBUFSIZE];   //Buffer for echo string
    memset(&echoBuffer,0,RCVBUFSIZE);
    int recvMsgSize;
    
    //recieve message from client
    if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0))<0)
        DieWithError("recv() failed");
    
    // Send received string and receive again until end of transmission
    while(recvMsgSize>0)
    {
        //Echo message back to client
            if(send(clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
                DieWithError("send() failed");
        
            // See if there is more data to receive
            if((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) <0)
                DieWithError("recv() failed");
    }
    
    printf("echoBuffer:%s\n", echoBuffer);
    /*int size= (sizeof(echoBuffer)/sizeof(echoBuffer[0]));
    printf("Size: %u", size);*/
    if(echoBuffer[0]=='s')
        printf("hell yeah\n");
    //handleCommand(echoBuffer)
    
    printf("close:\n");
    close(clntSocket);
}

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
    
    
    
