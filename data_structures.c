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
#define TABLE_SIZE 16

node_information* set_up_node(int tcp_port,char*tcp_ip,char*reg_ip,int reg_UDP)
{
    node_information* node_info;
    struct addrinfo * server_info=NULL;
    char str[6];

    node_info=(node_information*)malloc(sizeof(node_information));
    if(node_info==NULL)
    {
        perror("error in node_info allocation\n");
    }

    node_info->fd =(int*) malloc(sizeof(int)*TABLE_SIZE);
    node_info->port =(int*) malloc(sizeof(int)*CONNECTIONS_NUM);
    node_info->ip =(char**) malloc(sizeof(char*)*CONNECTIONS_NUM);
    node_info->destinations = (int*) malloc(sizeof(int)*TABLE_SIZE);
    node_info->neighbours = (int*) malloc(sizeof(int)*TABLE_SIZE);

    /*Initialize file descriptors to -1*/ 
    for(int i=0; i<CONNECTIONS_NUM; i++ ) 
    {
        node_info->ip[i] = (char*) malloc(16*sizeof(char));
        node_info->port[i]=-1;
        strcpy(node_info->ip[i],"000.000.000.000");
    }

    for(int i=0; i<TABLE_SIZE; i++)
    {
       node_info->fd[i] = -1; 
       node_info->destinations[i] =-1;
       node_info->neighbours[i]=-1;
    }

    /*Initialize node info*/
    node_info->ring=-1;
    node_info->chord_id=-1;

    strcpy(node_info->udp_ip,reg_ip);
    node_info->udp_port=reg_UDP;
    strcpy(node_info->tcp_ip,tcp_ip);
    node_info->tcp_port=tcp_port;

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

    /*Allocates memory for tables*/

    node_info->routing_table = (pathset***)malloc(sizeof(pathset**)*TABLE_SIZE);
    node_info->short_way = (pathset**)malloc(sizeof(pathset*)*TABLE_SIZE);
    node_info->expedition =(int*)malloc(sizeof(int)*TABLE_SIZE);


    for(int i=0; i<TABLE_SIZE; i++)
    {
        node_info->routing_table[i] =(pathset**)malloc(sizeof(pathset*)*TABLE_SIZE);
        node_info->short_way[i] =(pathset*)malloc(sizeof(pathset));
        sprintf(node_info->short_way[i]->field, "-");
        node_info->short_way[i]->n_fields=-1;
        node_info->expedition[i]=-1;


        for(int j=0; j<TABLE_SIZE; j++)
        {
            node_info->routing_table[i][j] =(pathset*)malloc(sizeof(pathset)*TABLE_SIZE);
            node_info->routing_table[i][j]->n_fields=-1;
            sprintf(node_info->routing_table[i][j]->field,"-");
        }

    }    

    /*Initialize own node values in tables*/
    node_info->short_way[0]->n_fields = 1;

    return node_info;
}

