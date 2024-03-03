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

#include "data_structures.h"
#include "udp_functions.h"


node_information* set_up_node(int id,int tcp_port,char*ip,char*reg_ip,int reg_UDP)
{
    node_information* node_info;
    struct addrinfo * server_info=NULL;
    char str[6];

    node_info=(node_information*)malloc(sizeof(node_information));
    if(node_info==NULL)
    {
        perror("error in node_info allocation\n");
    }

    node_info->id=id;
    strcpy(node_info->ip,ip);
    node_info->port=tcp_port;
    strcpy(node_info->udp_ip,reg_ip);
    node_info->udp_port=reg_UDP;

    sprintf(str,"%d",reg_UDP);

    server_info= UDPserver_info(server_info,reg_ip,str);

    node_info->udp_server_info=server_info;

    node_info->succ_fd=-1;
    node_info->pred_fd=-1;

    return node_info;
}