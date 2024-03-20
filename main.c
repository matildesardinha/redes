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

#include "check_usage.h"
#include "data_structures.h"
#include "tcp_functions.h"
#include "ring_message.h"
#include "udp_functions.h"
#include "commands.h"
#include "routing.h"

#define BUFFER_SIZE 200
#define NODES 16

int main (int argc, char **argv)
{
    int regUDP= 59000;
    char regIP[16]= "193.136.138.142";
    node_information * node_info;

    char buffer [BUFFER_SIZE];
    memset(buffer,0,BUFFER_SIZE);

    
    /*Check incorrect argument number*/
    if(argc!=3 && argc!=5)
    {
        fprintf(stderr,"Usage: %s <IP> <TCP> <regIP> <regUDP>\n", argv[0]);
        exit(1);
    }

    /*Check arguments format*/

    if(check_port(atoi(argv[2]))==0 || check_ipv4(argv[1])==0) 
    {
        exit(EXIT_FAILURE);
    }

    if(argc==5)
    {
       if(check_ipv4(argv[3]) || check_port(atoi(argv[4])))
       {
            exit(EXIT_FAILURE);
       }
    }
    
    /*Allocates space for the node and sets up inicial parameters*/
    if (argc==3)
    {
        node_info=set_up_node(atoi(argv[2]),argv[1],regIP,regUDP);
    }
    else
    {
        node_info=set_up_node(atoi(argv[2]),argv[1],argv[3],atoi(argv[4]));
    }

    /*Creates a TCP server for the node*/

    node_info->tcp_server_fd=tcp_server(argv[1],atoi(argv[2]));

    /*Select call*/

    int len=0, newfd, receive,i;
    struct sockaddr client_addr;
    socklen_t client_len= sizeof(client_len);

    FD_ZERO (&(node_info->readfds));
    FD_SET (STDIN_FILENO,&(node_info->readfds));
    FD_SET (node_info->tcp_server_fd,&(node_info->readfds));

    /*Set maxfd*/

    if(node_info->tcp_server_fd > STDIN_FILENO)
    {
        node_info->maxfd=node_info->tcp_server_fd;
    }
    else
    {
        node_info->maxfd= STDIN_FILENO;
    }

    
    while(1)
    {
        fd_set tmpfds;
        FD_ZERO(&tmpfds);
        tmpfds=node_info->readfds;

        if(select(node_info->maxfd + 1,&tmpfds,NULL,NULL,NULL)<0)
        {
            printf("Select() Error\n");
        }

        /*Check user input*/
        if (FD_ISSET(STDIN_FILENO, &tmpfds)) 
        {
            fgets(buffer, BUFFER_SIZE, stdin);
            len = strlen(buffer);

            if (buffer[len-1] == '\n') 
            {
                buffer[len-1] = '\0';
            }

            /*Process user command*/
            process_command(node_info, buffer);
        }

        /*Check new connections*/

        else if(FD_ISSET(node_info->tcp_server_fd,&tmpfds))
        {
            printf("checks new connection\n");   
            newfd=accept(node_info->tcp_server_fd,&client_addr,&client_len);
            if(newfd<0)
            {
                perror("Error in accept\n");
                exit(1);
            }

            receive=receive_tcp_message(newfd,buffer,BUFFER_SIZE);
            if(receive<0 || receive==0)
            {
                printf("Could not receive tcp message\n");
            }
            else
            {
                process_new_connection(node_info,buffer,newfd);

            }
            
        }
        /*Check successor*/
        else if(FD_ISSET(node_info->succ_fd,&tmpfds))
        {
            receive=receive_tcp_message(node_info->succ_fd,buffer,BUFFER_SIZE);
                    if(receive<0)
                    {
                        printf("Could not receive tcp message\n");
                    }
                     /*Node disconnected*/
                    else if(receive==0)
                    {
                        node_left(node_info,node_info->succ_id,node_info->ring);
                    }
                    else
                    {
                        process_tcp_message(node_info,buffer,node_info->succ_fd);
                    }               
        }
        /*Check predecessor*/
        else if(FD_ISSET(node_info->pred_fd,&tmpfds))
        {
            receive=receive_tcp_message(node_info->pred_fd,buffer,BUFFER_SIZE);
                    if(receive<0)
                    {
                        printf("Could not receive tcp message\n");
                    }
                     /*Node disconnected*/
                    else if(receive==0)
                    {
                        node_left(node_info,node_info->pred_id,node_info->ring);
                    }
                    else
                    {
                        process_tcp_message(node_info,buffer,node_info->succ_fd);
                    }               
        }

        /*Check chords*/
        else
        {
           for(i=0; i<NODES ; i++)
           {
            if(node_info->fd[i] != -1)
            {
                if(FD_ISSET(node_info->fd[i],&tmpfds))
                {
                  receive=receive_tcp_message(node_info->fd[i],buffer,BUFFER_SIZE);
                    if(receive<0)
                    {
                        printf("Could not receive tcp message\n");
                    }
                     /*Node disconnected*/
                    else if(receive==0)
                    {
                        close(node_info->fd[i]);
                        FD_CLR(node_info->fd[i],&(node_info->readfds));

                        /*Updates maxfd*/
                        if(node_info->maxfd==node_info->fd[i])
                        {
                          node_info->fd[i]=-1;    
                          find_new_max(node_info);
                        }
                        else
                        {
                           node_info->fd[i]=-1; 
                        }

                        node_left(node_info,node_info->neighbours[i],node_info->ring);
                    }
                    else
                    {
                        process_tcp_message(node_info,buffer,node_info->fd[i]);
                    }
                }
            }
           } 
        }
    }
}
