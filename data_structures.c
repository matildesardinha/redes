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

#define CONNECTIONS_NUM 100

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

    node_info->connection = (connections*) malloc(sizeof(connections));
    node_info->connection->fd =(int*) malloc(sizeof(int)*CONNECTIONS_NUM);
    node_info->connection->port =(int*) malloc(sizeof(int)*CONNECTIONS_NUM);
    node_info->connection->ip =(char**) malloc(sizeof(char*)*CONNECTIONS_NUM);

    for(int i=0; i<CONNECTIONS_NUM; i++ ) {
        node_info->connection->ip[i] = (char*) malloc(16*sizeof(char));
    }


    /*Initialize file descriptors to -1*/ 
    for(int i=0; i<CONNECTIONS_NUM; i++ ) 
    {
        node_info->connection->fd[i] = -1;
        node_info->connection->port[i]=-1;
        strcpy(node_info->connection->ip[i],"000.000.000.000");
    }

    /*Initialize node info*/
    node_info->ring=-1;
    node_info->id=id;
    strcpy(node_info->ip,ip);
    node_info->port=tcp_port;
    strcpy(node_info->udp_ip,reg_ip);
    node_info->udp_port=reg_UDP;

    /*Initialize SUCC*/
    node_info->succ_id=-1;
    node_info->succ_fd=-1;

    /*Initialize PRED*/    
    node_info->pred_id=-1;
    node_info->pred_fd=-1;

    /*Initialize S_SUCC*/
    node_info->s_succ_id=-1;

    sprintf(str,"%d",reg_UDP);

    server_info= UDPserver_info(server_info,reg_ip,str);

    node_info->udp_server_info=server_info;

    return node_info;
}