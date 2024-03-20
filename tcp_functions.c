#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#include "tcp_functions.h"

#define BUFFER 200

int tcp_server(char *ip, int port)
{
    int fd, errcode;
    struct addrinfo hints, *res;
    char port_s[7];

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
       perror("error tcp_server:socket:\n"); 
       exit(1); 
    } 

    /*Converts port into string*/
    sprintf(port_s,"%d",port);

    memset (&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(ip,port_s,&hints,&res);
    if((errcode!=0))
    {
        perror("error tcp_server: getaddrinfo\n");
        exit(1);
    } 

    if(bind(fd,res->ai_addr,res->ai_addrlen)<0)
    {
        perror("error in tcp_server bind\n");
        exit(1);
    }

    if(listen(fd,99)<0)
    {
        perror("error in tcp_server listen\n");
        exit(1); 
    } 

    freeaddrinfo(res);

    return fd;
}


int send_tcp_message(int fd, char*message, int message_size)
{
    ssize_t bytes_written=0;
    
    while(bytes_written < message_size)
    {
        bytes_written += write(fd,message +bytes_written ,message_size-bytes_written); 
        if(bytes_written<=0)
        {
            perror("error in send_tcp_message write\n");
            exit(1);
        }
    }

    printf ("\nMESSAGE SENT: %s | bytes: %ld\n",message,bytes_written);

    return bytes_written;
}

int receive_tcp_message(int fd, char* buffer, int buffer_size) 
{
    ssize_t bytes_read = 0;

    /*Clear the buffer before using it*/
    memset(buffer, 0, buffer_size);

    /*Continue reading until buffer is full or newline is encountered*/
    while (bytes_read < buffer_size - 1) {
        /*Read one byte at a time*/ 
        ssize_t bytes = read(fd, buffer + bytes_read, 1);
        if (bytes == -1 || bytes == 0) {
            break;
        }
        bytes_read += bytes;

        /*Check if the received data contains a newline character*/ 
        if (buffer[bytes_read - 1] == '\n') {
            buffer[bytes_read - 1] = '\0';  /*Terminate the string at the newline*/ 
            break;  /*Exit loop once newline is found*/ 
        }
    }

    printf("\nMESSAGE RECEIVED: %s  |  Bytes: %ld\n", buffer, bytes_read);
    return bytes_read;
}


int tcp_client (char *ip, int port)
{
    ssize_t n;
    int fd, errcode;
    struct addrinfo hints, *res;
    char port_s [7];

    /*Convert port into a string for getaddrinfo*/
    sprintf(port_s,"%d",port);

    fd=socket(AF_INET, SOCK_STREAM,0);
    if(fd==-1)
    {
       perror("error tcp_client socket");
       exit(1); 
    } 

    /*Converts port into string*/
    sprintf(port_s,"%d",port);

    memset (&hints,0,sizeof(hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(ip,port_s,&hints,&res);
    if((errcode!=0))
    {
        perror("error tcp_server: getaddrinfo\n");
        exit(1);
    } 

    n=connect(fd,res->ai_addr,res->ai_addrlen);

    if(n==-1)
    {
        perror("error in tcp_client connect\n");
        exit(1);
    }

    freeaddrinfo(res);

    return fd;
}