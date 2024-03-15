#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>

#include "commands.h"
#include "udp_functions.h"
#include "tcp_functions.h"
#include "ring_message.h"

#define BUFFER 200

#define NODES_BUFFER 300

#define MAX_ARG 4

/*CUIDADO STRCMP*/

void process_command(node_information* node_info, char *buffer)
{
    char *command, *arguments[MAX_ARG];
    int num_args;
     
    command = strtok(buffer, " ");
    num_args = 0;

    while (num_args <= MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }

    /*No command introduced*/
    if (command == NULL) {
        printf("No command inserted.\n"); /*não sei se isto faz muito sentido*/
        return;
    }

    if((strcmp(command,"join")==0) && num_args==2)
    {
        join(node_info,atoi(arguments[0]),atoi(arguments[1]));
    }
    
    else if ((strcmp(command,"dj")==0) && num_args==4)
    {
        djoin(node_info,atoi(arguments[0]),atoi(arguments[1]),arguments[2],atoi(arguments[3]));
    }
    else if ((strcmp(command,"st")==0) && num_args==0)
    {
        show_topology(node_info);
    }
    else if((strcmp(command,"leave")==0) && num_args==0)
    {
        leave(node_info);
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

    /*Update SUCC*/
    node_info->id=id;
    node_info->succ_id=succ_id;
    strcpy(node_info->connection->ip[succ_id],succ_ip);
    node_info->connection->port[succ_id]=succ_port;


    /*Client connection*/
    fd = tcp_client (succ_ip, succ_port);

    node_info->succ_fd=fd;

    FD_SET(node_info->succ_fd,&(node_info->readfds));

    if(fd>node_info->maxfd)
    {
        node_info->maxfd=fd;
    }

    /*Send entry message to the successor*/
    ENTRY(fd,node_info->id,node_info->ip,node_info->port);

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
    server_response=message_serverUDP(node_info->udp_server_info,buffer,strlen(buffer),nodes_buffer,NODES_BUFFER);

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

        printf("Selected node: %d %s %d\n",selected_id,selected_ip,selected_port);

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
    int pred_id,succ_id,s_succ_id,succ_port,s_succ_port;
    char succ_ip[16],s_succ_ip[16];

    pred_id=node_info->pred_id;
    succ_id=node_info->succ_id;
    s_succ_id=node_info->s_succ_id;

    /*Case node is alone*/
    if(pred_id==-1 && succ_id==-1)
    {
       succ_port=-1;
       s_succ_port=-1; 
       strcpy(succ_ip,"empty");
       strcpy(s_succ_ip,"empty");
    }
    else
    {
        succ_port=node_info->connection->port[succ_id];
        s_succ_port=node_info->connection->port[s_succ_id];
        strcpy(succ_ip,node_info->connection->ip[succ_id]);
        strcpy(s_succ_ip,node_info->connection->ip[s_succ_id]);
    }
    
    printf("-----Show topology-----\nNode: %d %s %d\n1st successor: %d %s %d\n2nd successor: %d %s %d\nPredecessor: %d\n-----------------------\n",
    node_info->id,node_info->ip,node_info->port,succ_id,succ_ip,succ_port,
    s_succ_id,s_succ_ip,s_succ_port,pred_id);

    printf("FDS: fd_pred:%d , fd_succ:%d", node_info->pred_fd,node_info->succ_fd);

    return;
}

void leave(node_information *node_info)
{
    int n;
    /*Checks if node is in a ring*/
    if(node_info->ring==-1)
    {
        printf("The node does not belong to a ring\n");
    }
    else
    {
        /*Send UNREG*/
        n=UNREG (node_info);

        if(n==0)
        {
            printf("Failed to leave ring\nUNREG wasn't send\n");
        }
        else
        {
            /*Close connection with successor*/
            if(node_info->succ_id!=-1)
            {
                close(node_info->succ_fd);
                FD_CLR(node_info->succ_fd,&(node_info->readfds));
            }

            /*Close connection with predecessor*/
            if(node_info->pred_fd!=-1)
            {
                close(node_info->pred_fd);
                FD_CLR(node_info->pred_fd,&(node_info->readfds));
            }
            
            /*Close connections with chords*/
            for (int i=1; i<100; i++)
            {
                if(node_info->connection->fd[i] != -1) 
                {
                    close(node_info->connection->fd[i]);
                    FD_CLR(node_info->connection->fd[i], &(node_info->readfds));
                }
            }
            
            /*Clear all the node info and reset initial values*/
            clear_node(node_info);
        }
    }

    return;
}

void node_left(node_information*node_info,int id,int ring)
{
    /*Case 2 nodes ring*/
    if(node_info->id==node_info->s_succ_id)
    {
        printf("Case 2 nodes ring\n");

        /*Close connection with successor*/
        close(node_info->succ_fd);
        FD_CLR(node_info->succ_fd,&(node_info->readfds));

        /*Clear node*/
        clear_node(node_info);

        /*Node still belongs in the ring*/
        node_info->ring=ring;
    }
    /*Case node was a SUCC*/
    else if (node_info->succ_id==id)
    {
        /*Close connection with node that left*/
        close(node_info->succ_fd);
        FD_CLR(node_info->succ_fd,&(node_info->readfds));

        /*Updates maxfd*/
        if(node_info->maxfd==node_info->succ_fd)
        {
            node_info->succ_fd=-1;    
            find_new_max(node_info);
        }
        else
        {
            node_info->succ_fd=-1; 
        }        

        printf("Case node was a SUCC\n");
        /*Creates TCP client with 2nd succ*/
        int fd;
        fd= tcp_client(node_info->connection->ip[node_info->s_succ_id],node_info->connection->port[node_info->s_succ_id]);

        /*Updates SUCC*/
        node_info->succ_fd=fd;
        FD_SET(node_info->succ_fd,&(node_info->readfds));

        if(node_info->succ_fd>node_info->maxfd){node_info->maxfd=node_info->succ_fd;}

        node_info->succ_id=node_info->s_succ_id;

        /*Sends SUCC*/
        SUCC(node_info->pred_fd,node_info->succ_id,node_info->connection->ip[node_info->succ_id],node_info->connection->port[node_info->succ_id]);

        /*Sends PRED*/
        PRED(node_info->succ_fd,node_info);
    }
    /*Case node was PRED*/
    else if(node_info->pred_id==id)
    {
        printf("Case node was a PRED\n");
        close(node_info->pred_fd);
        FD_CLR(node_info->pred_fd,&(node_info->readfds));

        /*Updates maxfd*/
        if(node_info->maxfd==node_info->pred_fd)
        {
            node_info->pred_fd=-1;    
            find_new_max(node_info);
        }
        else
        {
             node_info->pred_fd=-1; 
        }
    }
    /*Case node was a chord*/
    else
    {
        printf("node leaving was a chord\n");
    }
  
    return;
}

void clear_node(node_information*node_info)
{
    int i; 

    node_info->pred_fd=-1;
    node_info->pred_id=-1;
    node_info->ring=-1;
    node_info->s_succ_id=-1;
    node_info->succ_fd=-1;
    node_info->succ_id=-1;

    for (i=0 ;i<100; i++)
    {
        node_info->connection->fd[i]=-1;
        node_info->connection->port[i]=-1;
        strcpy(node_info->connection->ip[i],"000.000.000.000");
    }
    
    return;
}