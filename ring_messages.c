#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "ring_message.h"
#include "tcp_functions.h"
#include "udp_functions.h"
#include "data_structures.h"


#define BUFFER_SIZE 200

void ENTRY (int fd,int new_id,char*new_ip,int new_port)
{
    ssize_t n;
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "ENTRY %d %s %d\n",new_id,new_ip,new_port);
    n=send_tcp_message(fd,buffer,sizeof(buffer));   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;
}

void SUCC (int fd, int succ_id, char*succ_ip, int succ_port)
{
    ssize_t n;
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "SUCC %d %s %d\n",succ_id,succ_ip,succ_port);
    n=send_tcp_message(fd,buffer,sizeof(buffer));   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;
}

void PRED (int fd, node_information *node_info)
{
    ssize_t n;
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "PRED %d\n",node_info->id);
    n=send_tcp_message(fd,buffer,sizeof(buffer));   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;    
}

int REG (node_information*node_info)
{
    int n;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sprintf(message, "REG %d %d %s %d\n", node_info->ring, node_info->id,node_info->ip,node_info->port);
    n = message_serverUDP(node_info->udp_server_info,message,(size_t)sizeof(message),buffer,sizeof(buffer));
    
    if (n == 0 || strcmp(buffer, "OKREG") != 0)
    {
        printf("REG error: failed to register node.\n");
        return 0;
    }

    return 1;
}