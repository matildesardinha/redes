#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/select.h>

#include "udp_functions.h"

int main (int argc, char **argv)
{
    struct addrinfo *udp_server_info=NULL;
    char ip[16]="193.136.138.142"; 
    char port[6]="59000";
    char message[]="UNREG 061 40\n";
    char buffer[128];
    int result=-1;

    udp_server_info=UDPserver_info(udp_server_info, ip, port);
    result=message_serverUDP (udp_server_info, message, (size_t)strlen(message),buffer,strlen(buffer));

    printf("Resposta: \n%sRetorno: %d\n",buffer,result);
}