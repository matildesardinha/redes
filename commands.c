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
#include "routing.h"

#define BUFFER 200

#define NODES_BUFFER 300

#define MAX_ARG 4

#define NODES 16


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

    if((strcmp(command,"join")==0 || strcmp(command,"j")==0) && num_args==2)
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
    else if((strcmp(command,"leave")==0 || strcmp(command,"l")==0) && num_args==0)
    {
        leave(node_info);
    }
    else if((strcmp(command,"sp")==0) && num_args==1)
    {
        show_path(node_info,atoi(arguments[0]));
    }
    else if((strcmp(command,"sf")==0) && num_args==0)
    {
        show_forwarding(node_info);
    }
    else if((strcmp(command,"sr")==0) && num_args==1)
    {
        show_routing(node_info,atoi(arguments[0]));
    }
    else if((strcmp(command,"message")==0 || strcmp(command,"m")==0) && num_args==2)
    {
        send_chat(node_info,node_info->id,atoi(arguments[0]),arguments[1]);
    }
    else if((strcmp(command,"chord")==0 || strcmp(command,"c")==0) && num_args==0)
    {
        chord(node_info);
    }
    else if(strcmp(command,"rc")==0 && num_args==0)
    {
        remove_chord(node_info);
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

    /*Update SUCC*/
    node_info->succ_id=succ_id;
    strcpy(node_info->ip[succ_id],succ_ip);
    node_info->port[succ_id]=succ_port;

    /*Client connection*/
    fd = tcp_client (succ_ip, succ_port);

    node_info->succ_fd=fd;

    FD_SET(node_info->succ_fd,&(node_info->readfds));

    if(fd>node_info->maxfd)
    {
        node_info->maxfd=fd;
    }

    /*Send entry message to the successor*/
    ENTRY(fd,node_info->id,node_info->tcp_ip,node_info->tcp_port);

    /*Send shortest paths table*/
    send_route_new_connect(fd,node_info);   

    return;
}

void join(node_information *node_info,int ring, int id)
{   
    int server_response,num_nodes=0, selected_node, selected_id, selected_port;
    char selected_ip[16];

    char buffer[BUFFER],nodes_buffer[NODES_BUFFER];
    char *nodeslist[100];

    int ring_ids[100], i=0, id_read=0;

    /*Set up info about the node*/
    node_info->ring=ring;
    node_info->id=id;
    node_info->destinations[0]=id;
    node_info->expedition[0]=id;

    sprintf(node_info->short_way[0]->field,"%d",id);


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
    if(num_nodes==NODES)
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
            while(ring_ids[i]==1){ i++; }

            /*Update tables*/
            node_info->id=i;
            node_info->destinations[0]=i;
            node_info->expedition[0]=i;

            sprintf(node_info->short_way[0]->field,"%d",i);

            printf("join: the select ID already exists. Assigned ID: %02d\n",i);
        }

        /*Select node to connect to*/
        selected_node=(rand()%(num_nodes-1))+1;
        sscanf(nodeslist[selected_node],"%d %s %d", &selected_id,selected_ip,&selected_port);

        printf("Selected node: %d %s %d\n",selected_id,selected_ip,selected_port);

        /*Direct Join*/

        djoin(node_info,node_info->id,selected_id,selected_ip,selected_port);
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
        succ_port=node_info->port[succ_id];
        s_succ_port=node_info->port[s_succ_id];
        strcpy(succ_ip,node_info->ip[succ_id]);
        strcpy(s_succ_ip,node_info->ip[s_succ_id]);
    }
    
    printf("-----Show topology-----\nNode: %d %s %d\n1st successor: %d %s %d\n2nd successor: %d %s %d\nPredecessor: %d\n-----------------------\n",
    node_info->id,node_info->tcp_ip,node_info->tcp_port,succ_id,succ_ip,succ_port,
    s_succ_id,s_succ_ip,s_succ_port,pred_id);

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
            for (int i=1; i<NODES; i++)
            {
                if(node_info->fd[i] != -1) 
                {
                    close(node_info->fd[i]);
                    FD_CLR(node_info->fd[i], &(node_info->readfds));
                }
            }
            
            /*Clear all the node info and reset initial values*/
            clear_node(node_info);
        }
    }

    return;
}

void chord (node_information*node_info)
{
    char nodes_buffer[NODES_BUFFER];
    char buffer[BUFFER], ips[NODES][20], *nodeslist[NODES];
    int server_response, ids[NODES],ports[NODES],num_nodes=0,selected_node, chord_fd, neigh_place, lines=0, valid=0,i;

    if(node_info->chord_id == -1)
    {
        /*Connect node server*/
        sprintf(buffer,"NODES %03d", node_info->ring);
        server_response=message_serverUDP(node_info->udp_server_info,buffer,strlen(buffer),nodes_buffer,NODES_BUFFER);

        if (server_response==0)
        {
            printf("Could not connect to node server");
            return;
        }

        /*Parsing*/
        strtok(nodes_buffer,"\n");
        nodeslist[lines]=strtok(NULL,"\n");

        while (nodeslist[lines]!=NULL && num_nodes<NODES)
        {
            printf("Nodeslist[lines]%s\n",nodeslist[lines]);

            sscanf(nodeslist[lines], "%d %s %d\n", &ids[num_nodes], ips[num_nodes], &ports[num_nodes]);

            printf("O que foi guardado: %d %s %d\n",ids[num_nodes],ips[num_nodes],ports[num_nodes]);
            
            if (!(ids[num_nodes] == node_info->id || ids[num_nodes] == node_info->pred_id || ids[num_nodes] == node_info->succ_id))
            {
                // Condition met, so increment num_nodes
                num_nodes++; 
            }

        lines++;
        nodeslist[lines]=strtok(NULL,"\n");  
        }
        printf("Numero de linhas lido: %d\n",lines);
        printf("Numero de nos: %d\n",num_nodes);

        /*Can only create a chord if there are at least 4 nodes in the ring*/
        if(lines>3)
        {
            while(valid != 1 || num_nodes == 0)
            {
                /*Select a random node to create a chord with*/
                selected_node=(rand()%(num_nodes));

                i=find(selected_node,node_info->neighbours);
                if(i==-1)
                {
                    valid = 1;
                }
                else if(node_info->fd[i]==-1)
                {
                    valid = 1;
                }
                else
                {
                    --num_nodes;
                }
            }

            /*If it is a valid node to connect to*/
            if(valid==1)
            {
                /*Create TCP client*/
                chord_fd=tcp_client(ips[selected_node],ports[selected_node]);

                neigh_place=add_neighbour(node_info,ids[selected_node]);
                node_info->chord_id=ids[selected_node];

                FD_SET(chord_fd,&(node_info->readfds));
                if(chord_fd>node_info->maxfd)
                {
                    node_info->maxfd=chord_fd;
                }

                node_info->fd[neigh_place]=chord_fd;

                /*Sends chord message*/
                CHORD(chord_fd,node_info);

                /*Sends tables*/
                send_route_new_connect(chord_fd,node_info);
            }
        }
        else
        {
            printf("There are not enough nodes on the ring to create a chord\n");
        }
    }
    else
    {
        printf("Node already created a chord\n");
    }
    return;
}

void receive_chord(node_information* node_info, int node, int fd)
{
    int neigh_place,i;

    i=find(node,node_info->neighbours);
    if(i==-1)
    {
        /*Add chord to neighbours*/
        neigh_place=add_neighbour(node_info,node);
        node_info->fd[neigh_place]=fd;
    }

    /*Only accepts connection if it isn't already a chord*/
    if(node_info->fd[neigh_place]==-1)
    {
        FD_SET(node_info->fd[neigh_place],&(node_info->readfds));
        if(node_info->maxfd < node_info->fd[neigh_place])
        {
            node_info->maxfd = node_info->fd[neigh_place];
        }

        /*Send routes to the new connection*/
        send_route_new_connect(node_info->fd[neigh_place],node_info);
    }

    return;
}

void remove_chord(node_information *node_info)
{
    int neigh_place;

    neigh_place=find(node_info->chord_id,node_info->neighbours);

    /*Close connection*/
    close(node_info->fd[neigh_place]);
    FD_CLR(node_info->fd[neigh_place],&(node_info->readfds));

    /*Calculate new maxfd if needed*/
    if(node_info->fd[neigh_place] == node_info->maxfd)
    {
        node_info->fd[neigh_place]=-1;
        find_new_max(node_info);
    }
    else
    {
        node_info->fd[neigh_place]=-1;  
    }
    
    update_tables_after_remove(node_info->chord_id,node_info);
    node_info->chord_id=-1;

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
        printf("Case node was a SUCC\n");
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

        /*Creates TCP client with 2nd succ*/
        int fd;
        fd= tcp_client(node_info->ip[node_info->s_succ_id],node_info->port[node_info->s_succ_id]);

        /*Updates SUCC*/
        node_info->succ_fd=fd;
        FD_SET(node_info->succ_fd,&(node_info->readfds));

        if(node_info->succ_fd>node_info->maxfd){node_info->maxfd=node_info->succ_fd;}

        node_info->succ_id=node_info->s_succ_id;

        /*Sends SUCC*/
        SUCC(node_info->pred_fd,node_info->succ_id,node_info->ip[node_info->succ_id],node_info->port[node_info->succ_id]);

        /*Sends PRED*/
        PRED(node_info->succ_fd,node_info);

        /*Update tables*/
        update_tables_after_remove(id,node_info);

        /*Sends shortest path table*/
        send_route_new_connect(fd,node_info);

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

        update_tables_after_remove(id,node_info);
    }
    /*Case node was a chord*/
    else
    {
        printf("node leaving was a chord\n");

        /*Updates tables after remove*/
        update_tables_after_remove(id,node_info);
    }
  
    return;
}

void clear_node(node_information*node_info)
{
    int i,j;

    node_info->pred_fd=-1;
    node_info->pred_id=-1;
    node_info->ring=-1;
    node_info->s_succ_id=-1;
    node_info->succ_fd=-1;
    node_info->succ_id=-1;

    
    for(i=0; i<NODES; i++)
    {
        node_info->destinations[i]=-1;
        node_info->fd[i]=-1;       
    }
    for (i=0 ;i<100; i++)
    {
        node_info->port[i]=-1;
        strcpy(node_info->ip[i],"000.000.000.000");
    }

    /*Clear tables*/
    for(i=0; i<NODES ; i++)
    {
        node_info->destinations[i]=-1;
        node_info->neighbours[i]=-1;
        node_info->expedition[i]=-1;
        node_info->short_way[i]->n_fields=-1;
        strcpy(node_info->short_way[i]->field,"-");

        for(j=0; j<NODES ; j++)
        {
            node_info->routing_table[i][j]->n_fields=-1;
            strcpy(node_info->routing_table[i][j]->field,"-");
        }
    }
    
    return;
}