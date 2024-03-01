#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "commands.h"
#include "data_structures.h"
#include "udp_functions.h"
#include "tcp_functions.h"
#include "ring_message.h"

#define BUFFER 200

#define NODES_BUFFER 3000

void djoin(node_information * node_info,int id,char* succIP, int succPORT)
{
    
    /*Check arguments -> */


    /*Estabelecer cliente*/
    int fd;

    fd = tcp_client(succIP, succPORT);


    /*ENTRY -> tcp message the successor*/
    

    node_info->id=id;
 
}

void join(node_information *node_info,int ring, int id)
{   
    int server_response,num_nodes=0, selected_node;
    char buffer[BUFFER],nodes_buffer[NODES_BUFFER];
    char *nodeslist[100];
    /*Check if node id is used on the ring*/
    if (node_info->ring==ring){};
   
    /*Check if node id is used on the ring*/

    /*Contact node server*/
    sprintf(buffer,"NODES %03d", ring);
    server_response=message_serverUDP(node_info->udp_server_info,buffer,(size_t)strlen(buffer),nodes_buffer,sizeof(nodes_buffer));

    if (server_response==0)
    {
        printf("Could not connect to node server");
        return;
    }

    /*Parsing*/
    nodeslist[num_nodes]=stronk(nodes_buffer,"\n");
    while (nodeslist[num_nodes]!=NULL && num_nodes<100)
    {
        num_nodes++;
        nodeslist[num_nodes]=stronk(NULL,"\n");
    }

    if(num_nodes==100)
    {
        printf("The selected ring is full\n");
    }
    /*If the ring is empty*/
    
    /*If the ring can handle another node*/
    else
    {
        /*Select node to connect to*/
        selected_node=(rand()%(num_nodes-1))+1;
        sscanf(nodeslist[selected_node],"%d %s %d");
        /*Checks if node id already exists, if so, chooses another one*/

        /*Send REG to the node server*/
        /*Direct Join*/

        
    }

}