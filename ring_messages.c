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


#define BUFFER_SIZE 200

ssize_t ENTRY (int fd,int new_id,char*new_ip,int new_port)
{
    ssize_t n;
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "ENTRY %d %s %d\n",new_id,new_ip,new_port);
    n=send_tcp_message(fd,buffer,sizeof(buffer));   

    return n;
}