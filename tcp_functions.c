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
    int bytes_written=0;
    
    while(bytes_written < message_size)
    {
        bytes_written += write(fd,message +bytes_written ,message_size-bytes_written); 
        if(bytes_written<=0)
        {
            perror("error in send_tcp_message write\n");
            exit(1);
        }
    }

    printf ("Mensagem enviada: %s | bytes: %d\n",message,bytes_written);

    return bytes_written;
}

int receive_tcp_message(int fd, char* buffer, int buffer_size)
{
    int bytes_read=0;
    char* end_of_message=NULL;

  
    while (end_of_message == NULL)
    {
        bytes_read +=read(fd,buffer + bytes_read,buffer_size-bytes_read);

        if(bytes_read==-1 || bytes_read==0)
        {
            break;
        }

        end_of_message= strrchr (buffer, '\n');
        if(end_of_message !=NULL)
        {
            *end_of_message= '\0'; 
        }

         printf("\nMESSAGE RECEIVED: %s  |  Bytes: %d\n", buffer, bytes_read);    
    }

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