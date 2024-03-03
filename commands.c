#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include "commands.h"
#include "udp_functions.h"
#include "tcp_functions.h"
#include "ring_message.h"

#define BUFFER 200

#define NODES_BUFFER 3000

#define MAX_ARG 4

void process_command(node_information* node_info, char *buffer)
{
    char *command, *arguments[MAX_ARG];
    int num_args;
     
    command = strtok(buffer, " ");
    num_args = 0;

    while (num_args < MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }

    /*No command introduced*/
    if (command == NULL) {
        printf("No command inserted.\n"); /*não sei se isto faz muito sentido*/
        return;
    }

    if((strcmp(command,"join") || strcmp(command,"j")) && num_args==2)
    {
        join(node_info,atoi(arguments[0]),atoi(arguments[1]));
    }
    
    else if ((strcmp(command,"direct join") || strcmp(command,"dj")) && num_args==4)
    {
        djoin(node_info,atoi(arguments[0]),atoi(arguments[1]),arguments[2],atoi(arguments[3]));
    }
    else if ((strcmp(command,"show topology") || strcmp(command,"st")) && num_args==0)
    {
        show_topology(node_info);
    }

    else
    {
        printf("Invalid command\n");
    }
    return;
}

void djoin(node_information * node_info,int id,int succ_id,char* succ_ip, int succ_port)
{
    int fd;

    /*Check arguments*/

    /*Update info*/
    node_info->id=id;
    node_info->succ_id=succ_id;
    strcpy(node_info->succ_ip,succ_ip);
    node_info->succ_port=succ_port;


    /*Client connection*/
    fd = tcp_client(succ_ip, succ_port);

    /*Send entry message to the successor*/
    ENTRY(fd,node_info->id,node_info->ip,node_info->port);

    close(fd);

    return;
}

void join(node_information *node_info,int ring, int id)
{   
    int server_response,num_nodes=0, selected_node, selected_id, selected_port;
    char selected_ip[16];

    char buffer[BUFFER],nodes_buffer[NODES_BUFFER];
    char *nodeslist[100];

    int ring_ids[100], i=0, id_read=0;

    node_info->ring=ring;
    node_info->id=id;

    /*Contact node server*/
    sprintf(buffer,"NODES %03d", ring);
    server_response=message_serverUDP(node_info->udp_server_info,buffer,(size_t)strlen(buffer),nodes_buffer,sizeof(nodes_buffer));

    if (server_response==0)
    {
        printf("Could not connect to node server");
        return;
    }

    /*Ring Ids is set to:
    -0 if the id doesn´t exist on the ring
    -1 if it does
    */

    for(i=0; i<100; i++)
    {
        ring_ids[i]=0;
    }

    /*Parsing*/
    nodeslist[num_nodes]=strtok(nodes_buffer,"\n");

    while (nodeslist[num_nodes]!=NULL && num_nodes<100)
    {
        sscanf(nodeslist[num_nodes],"%d",&id_read);
        ring_ids[id_read]=1; 

        num_nodes++;
        nodeslist[num_nodes]=strtok(NULL,"\n");    
    }

    /*Case: the ring is full*/
    if(num_nodes==100)
    {
        printf("The selected ring is full\n");
    }

    /*Case: the ring is empty*/
    else if(num_nodes==1)
    {
        printf("Ring is empty\n");
        REG (node_info);
        printf("REG done\n");
    }


  
    /*Case: node will conect to another node*/
    else
    {
        printf("Node will connect to another node\n");
        /*Selects another ID for the node if the current one is being used*/
        if(ring_ids[id]==1)
        {
            i=0;
            while(ring_ids[i]==1)
            {
                i++;
            }
            node_info->id=i;
            printf("join: the select ID already exists. Assigned ID: %02d\n",i);
        }

        /*Select node to connect to*/
        selected_node=(rand()%(num_nodes-1))+1;
        sscanf(nodeslist[selected_node],"%d %s %d", &selected_id,selected_ip,&selected_port);

        /*Direct Join*/

        djoin(node_info,id,selected_id,selected_ip,selected_port);
        printf("djoin done\n");

        /*Send REG to the node server*/
        
        REG (node_info);       
        printf ("REG done\n"); 
    }

    return;
}

void show_topology(node_information *node_info)
{
    
    printf("Show topology:%d %s %d\n1st successor %d %s %d\n2nd successor %d %s %d\npredecessor %d\n",
    node_info->id,node_info->ip,node_info->port,node_info->succ_id,node_info->succ_ip,node_info->succ_port,
    node_info->s_succ_id,node_info->s_succ_ip,node_info->s_succ_port,node_info->pred_id);

    return;
}