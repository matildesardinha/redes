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

#include "udp_functions.h"


struct addrinfo* UDPserver_info(struct addrinfo *udp_server_info, char *ip, char *port)
{
    int errcode;
    struct addrinfo hints;

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_INET; /*IPv4*/
    hints.ai_socktype=SOCK_DGRAM; /*UDP socket*/

    errcode=getaddrinfo(ip,port,&hints,&udp_server_info);
    
    /*Error*/
    if(errcode!=0)
    {
      perror("Error getaddrinfo:");  
      exit(1);  
    } 

    return udp_server_info;
}

int message_serverUDP (struct addrinfo *res,char *message, size_t message_size,char*buffer,int buffer_size)
{
    int fd,attempts=0,n=0;
    ssize_t n_sent=0, n_received=0;
    socklen_t addrlen;
    struct sockaddr addr;

    fd_set readfds;
    struct timeval timeout;

    fd=socket(AF_INET, SOCK_DGRAM,0); /*UDP socket*/

    /*Error*/
    if(fd==-1)
    {
        perror("Socket error:");
        exit(1);
    }

    FD_ZERO(&readfds);
    FD_SET(fd,&readfds);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    while(attempts<2)
    {
        /*Message node server*/
        
        n_sent=sendto(fd,message,message_size,0,res->ai_addr,res->ai_addrlen);
        
        if(n_sent==-1)
        {
            perror ("Error sento:");
            break;
        }

        /*Wait for reply*/
        n=select(fd+1, &readfds,NULL,NULL,&timeout);

        if(n==-1)
        {
            perror("Error message_serverUDP: select()");
            break;
        }

        else if(n==0)
        {
            attempts++;
        }

        else
        {
            addrlen=sizeof(addr);
            n_received=recvfrom(fd,buffer,buffer_size-1,0,&addr,&addrlen);
            if(n_received==-1)
            {
                perror("Error message_serverUDP recvfrom");
                break;
            }

            buffer[n_received]= '\0';
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}

    