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


#define BUFFER_SIZE 200
#define MAX_ARG 3


void ENTRY (int fd,int new_id,char*new_ip,int new_port)
{
    /*ssize_t n;*/
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "ENTRY %02d %s %d\n",new_id,new_ip,new_port);
    /*n=*/
    send_tcp_message(fd,buffer,strlen(buffer));   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;
}

void SUCC (int fd, int succ_id, char*succ_ip, int succ_port)
{
    /*ssize_t n;*/
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "SUCC %02d %s %d\n",succ_id,succ_ip,succ_port);
    /*n=*/
    send_tcp_message(fd,buffer,strlen(buffer));   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;
}

void PRED (int fd, node_information *node_info)
{
    /*ssize_t n;*/
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "PRED %02d\n",node_info->id);
    /*n=*/
    send_tcp_message(fd,buffer,BUFFER_SIZE);   

    /*Checkar se a totalidade da mensagem foi enviada*/

    return;    
}

int REG (node_information*node_info)
{
    int n;
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    

    sprintf(message, "REG %03d %02d %s %d\n", node_info->ring, node_info->id,node_info->ip,node_info->port);

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

void process_tcp_message(node_information*node_info, char*message, int fd)
{
    char *command, *arguments[MAX_ARG];
    char buffer[BUFFER_SIZE];
    int num_args;
     
    command = strtok(buffer, " ");
    num_args = 0;

    while (num_args <= MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }

    if(strcmp(command,"ENTRY") && num_args==3)
    {
        /*1st successor becomes 2nd successor
        Updates 2nd successor info*/
        node_info->s_succ_id = node_info->succ_id;
        strcpy(node_info->s_succ_ip,node_info->succ_ip);
        node_info->s_succ_port = node_info->succ_port;

        /*Updates 1st successor info*/
        node_info->succ_id = atoi(arguments[0]);
        strcpy(node_info->succ_ip,arguments[1]);
        node_info->succ_port = atoi(arguments[2]);
        node_info->succ_fd=fd;

        /*Sends SUCC to predecessor*/
        SUCC(node_info->pred_fd,node_info->succ_id,node_info->succ_ip,node_info->succ_port);

        /*Sends PRED to 1st successor*/
        PRED(node_info->succ_fd,node_info);
    }
    
    else if (strcmp(command,"SUCC") && num_args==3)
    {
        /*Updates 2nd successor*/
        node_info->s_succ_id = atoi(arguments[0]);
        strcpy(node_info->s_succ_ip,arguments[1]);
        node_info->s_succ_port = atoi(arguments[2]);
    }

    else if (strcmp(command,"PRED") && num_args==1)
    {
        /*Updates predecessor*/
        node_info->pred_id=atoi(arguments[0]);
        node_info->pred_fd=fd;
    }

    else
    {
        printf("Invalid ring message\n");
    }
    
    return;
}

int process_new_connection(node_information*node_info, char*message,int fd)
{
    char *command, *arguments[MAX_ARG];
    int num_args;
     
    command = strtok(message, " ");
    num_args = 0;

    while (num_args < MAX_ARG && (arguments[num_args] = strtok(NULL, " ")) != NULL) { 
        num_args++;
    }
    num_args=3;

    printf("Número de argumentos: %d\n",num_args);
    printf("Comando: %s\n",command);
    printf("\nargumento 0:%s\n",arguments[0]);

    if( !strcmp(command,"ENTRY") && num_args==3)
    {
        printf("\nVEM PARA AQUIII\n");
        /*Sends SUCC if node has successor*/
        if(node_info->succ_id != -1)
        {
            printf("\n SUCC entrou aqui\n");
            SUCC(fd,node_info->succ_id,node_info->succ_ip,node_info->succ_port);
        }

        /*Sends ENTRY if node has predecessor*/
        if(node_info->pred_id !=-1)
        {
            printf("\nENTRE entrou aqui\n");
           ENTRY(node_info->pred_fd,atoi(arguments[0]),arguments[1],atoi(arguments[2])); 
        }
        
        /*Update pred*/
        printf("\nPRED_ID:%d\n", atoi(arguments[0]));
        printf("\nPRED_FD:%d\n",fd);

        node_info->pred_id=atoi(arguments[0]);
        node_info->pred_fd=fd;

        printf("\ndor e sofrimento\n");
        return 1;
    }
    else
    {
        printf("Invalid ENTRY\n");
        return 0; 
    }

}