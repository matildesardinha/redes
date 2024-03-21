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
#include "routing.h"
#include "commands.h"


#define BUFFER_SIZE 200
#define MAX_ARG 3
#define NODES 16


void ENTRY (int fd,int new_id,char*new_ip,int new_port)
{

    char buffer[BUFFER_SIZE];

    sprintf(buffer, "ENTRY %02d %s %d\n",new_id,new_ip,new_port);

    send_tcp_message(fd,buffer,strlen(buffer));   


    return;
}

void SUCC (int fd, int succ_id, char*succ_ip, int succ_port)
{

    char buffer[BUFFER_SIZE];

    sprintf(buffer, "SUCC %02d %s %d\n",succ_id,succ_ip,succ_port);

    send_tcp_message(fd,buffer,strlen(buffer));   


    return;
}

void PRED (int fd, node_information *node_info)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "PRED %02d\n",node_info->id);

    send_tcp_message(fd,buffer,strlen(buffer));   

    return;    
}

void CHORD (int fd, node_information* node_info)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "CHORD %02d\n",node_info->id);

    send_tcp_message(fd,buffer,strlen(buffer));   

    return; 
}

int REG (node_information*node_info)
{
    int n;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sprintf(message, "REG %03d %02d %s %d\n", node_info->ring, node_info->id,node_info->tcp_ip,node_info->tcp_port);

    printf("%s",message);
    n = message_serverUDP(node_info->udp_server_info,message,strlen(message),buffer,BUFFER_SIZE);

    printf("Resposta ao REG : %s\n", buffer);
    
    if (n == 0 || strcmp(buffer, "OKREG") != 0)
    {
        printf("REG error: failed to register node.\n");
        return 0;
    }

    return 1;
}

int UNREG (node_information*node_info)
{
    int n;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    sprintf(message, "UNREG %03d %02d\n", node_info->ring, node_info->id);

    n = message_serverUDP(node_info->udp_server_info,message,strlen(message),buffer,BUFFER_SIZE);

    printf("Resposta ao UNREG : %s\n", buffer);
    
    if (n == 0 || strcmp(buffer, "OKUNREG") != 0)
    {
        printf("UNREG error: failed to unregister node.\n");
        return 0;
    }

    return 1;
}

void process_tcp_message(node_information*node_info, char*message, int fd)
{
    char *command, *arguments[MAX_ARG];
    int num_args;
     
    command = strtok(message, " ");
    num_args = 0;

    while (num_args < MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }

    if(!strcmp(command,"ENTRY") && num_args==3)
    {
        /*Close connection with succ*/
        close(node_info->succ_fd);
        FD_CLR(node_info->succ_fd,&(node_info->readfds));
        node_info->succ_fd=-1;

        /*If it's not the 2 nodes case, remove from the table*/
        if(node_info->id != node_info->s_succ_id)
        {
           update_tables_after_remove(node_info->succ_id,node_info);
        }

        /*Open connection with new succ*/
        node_info->succ_fd=tcp_client(arguments[1],atoi(arguments[2]));

        FD_SET(node_info->succ_fd,&(node_info->readfds));

        if(node_info->succ_fd > node_info->maxfd) {node_info->maxfd=node_info->succ_fd;}

        /*Sends PRED to succ*/
        PRED(node_info->succ_fd, node_info);

        /*Send route messages*/
        send_route_new_connect(node_info->succ_fd,node_info);
    
        /*1st successor becomes 2nd successor
        Updates 2nd successor info*/
        node_info->s_succ_id=node_info->succ_id;
        
        /*New entry becomes 1st successor
        Updates 1st successor info*/
        node_info->succ_id=atoi(arguments[0]);
        strcpy(node_info->ip[node_info->succ_id],arguments[1]);
        node_info->port[node_info->succ_id]=atoi(arguments[2]);

        /*Sends SUCC to predecessor*/
        SUCC(node_info->pred_fd,node_info->succ_id,node_info->ip[node_info->succ_id],node_info->port[node_info->succ_id]);
    }
    
    else if (!strcmp(command,"SUCC") && num_args==3)
    {
        /*Updates 2nd successor*/
        node_info->s_succ_id=atoi(arguments[0]);
        strcpy(node_info->ip[node_info->s_succ_id],arguments[1]);
        node_info->port[node_info->s_succ_id]=atoi(arguments[2]);
 
    }

    else if (!strcmp(command,"PRED") && num_args==1)
    {
        printf("Error: pred can only come from a new connection\n");
        
    }

    else if(!strcmp(command,"ROUTE") && num_args==3)
    {
        process_route(node_info,atoi(arguments[0]),atoi(arguments[1]),arguments[2]);
    }
    else if(!strcmp(command,"ROUTE") && num_args==2)
    {
        process_empty_route(node_info,atoi(arguments[0]),atoi(arguments[1]));
    }
    else if(!strcmp(command,"CHAT") && num_args==3)
    {
        receive_chat(node_info,atoi(arguments[0]),atoi(arguments[1]),arguments[2]);
    }
    else
    {
        printf("Invalid ring message\n");
    }
    
    return;
}

void process_new_connection(node_information*node_info, char*message,int fd)
{
    char *command, *arguments[MAX_ARG];
    int num_args, fd_succ;

    command = strtok(message, " ");
    num_args = 0;

    while (num_args < MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }

    if(strcmp(command,"ENTRY")==0)
    {
    
        /*Case is alone*/
        if(node_info->succ_id==-1)
        {
            /*Updates succ*/
            node_info->succ_id=atoi(arguments[0]);
            strcpy(node_info->ip[node_info->succ_id],arguments[1]);
            node_info->port[node_info->succ_id]=atoi(arguments[2]);

            /*Updates s_succ*/
            node_info->s_succ_id=node_info->id;
            strcpy(node_info->ip[node_info->s_succ_id],node_info->tcp_ip);
            node_info->port[node_info->s_succ_id]=node_info->tcp_port;

            /*Updates pred*/
            node_info->pred_id=atoi(arguments[0]);   
            node_info->pred_fd=fd;  

            FD_SET(node_info->pred_fd,&(node_info->readfds));

            if(node_info->pred_fd>node_info->maxfd)
            {
                node_info->maxfd=node_info->pred_fd;
            }     

            /*Sends SUCC*/
            SUCC(fd,node_info->succ_id,node_info->ip[node_info->succ_id],node_info->port[node_info->succ_id]);

            /*Creates TCP client*/
            fd_succ=tcp_client(arguments[1],atoi(arguments[2]));
            node_info->succ_fd=fd_succ;

            FD_SET(node_info->succ_fd,&(node_info->readfds));

            if(node_info->succ_fd>node_info->maxfd)
            {
                node_info->maxfd=node_info->succ_fd;
            }

            /*Sends PRED*/
            PRED(fd_succ,node_info);

            /*Sends shortest paths table*/
            send_route_new_connect(fd_succ,node_info);

        }
        /*Case node was not alone*/
        else
        {
            /*Removes old predecessor from tables if it isn't the 2 nodes case*/
            if(node_info->pred_id != node_info->succ_id)
            {
                update_tables_after_remove(node_info->pred_id,node_info); 
            }
            
            printf("process new connection node was not alone\n");
            /*Sends SUCC to the new node*/
            SUCC(fd,node_info->succ_id,node_info->ip[node_info->succ_id],node_info->port[node_info->succ_id]);

            /*Sends ENTRY to pred*/
            ENTRY(node_info->pred_fd,atoi(arguments[0]),arguments[1],atoi(arguments[2]));

            /*Updates predecessor*/
            node_info->pred_id=atoi(arguments[0]);  
            strcpy(node_info->ip[node_info->pred_id],arguments[1]);
            node_info->port[node_info->pred_id]=atoi(arguments[2]);
            node_info->pred_fd=fd;

            FD_SET(node_info->pred_fd,&(node_info->readfds));
            if( (node_info->pred_fd) > (node_info->maxfd))
            {
                node_info->maxfd = node_info->pred_fd;
            }

            /*Send shortest paths table*/
            send_route_new_connect(node_info->pred_fd,node_info);
        }

    }
    else if(strcmp(command,"PRED")==0)
    {
        /*Checks if there is a chord with the new predecessor*/
        if(node_info->chord_id==atoi(arguments[0]))
        {
            remove_chord(node_info);
        }
        
        /*Removes old predecessor from tables*/
        update_tables_after_remove(node_info->pred_id,node_info);

        /*Updates predecessor*/
        node_info->pred_id=atoi(arguments[0]);
        node_info->pred_fd=fd;

        FD_SET(node_info->pred_fd,&(node_info->readfds));
        if(node_info->pred_fd > node_info->maxfd)
        {
            node_info->maxfd= node_info->pred_fd;
        }

        /*Send SUCC*/
        SUCC(node_info->pred_fd,node_info->succ_id,node_info->ip[node_info->succ_id],node_info->port[node_info->succ_id]);

        /*Send shortest paths table*/
        send_route_new_connect(node_info->pred_fd,node_info);

        /*Two nodes case*/
        if(node_info->pred_id == node_info->succ_fd)
        {
            node_info->s_succ_id=node_info->id;
        }
    }
    else if(!strcmp(command,"CHORD"))
    {
        receive_chord(node_info, atoi(arguments[0]), fd);
    }
    else
    {
        printf("Invalid ENTRY\n"); 
    }
      return;
}

void find_new_max (node_information*node_info)
{
    int i;

    for(i=1; i<NODES; i++)
    {
        if (node_info->fd[i] > node_info->fd[i-1]) 
        { node_info->maxfd=node_info->fd[i];}
    }
    if (node_info->maxfd<node_info->succ_fd)
    {
        node_info->maxfd=node_info->succ_fd;
    }
    else if(node_info->maxfd<node_info->pred_fd)
    {
        node_info->maxfd=node_info->pred_fd;
    }
    else if(node_info->maxfd<node_info->tcp_server_fd)
    {
        node_info->maxfd=node_info->tcp_server_fd;
    }
    else if(node_info->maxfd<STDIN_FILENO)
    {
        node_info->maxfd=STDIN_FILENO;
    }
    
    return ;
}