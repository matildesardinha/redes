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


int main (int argc, char **argv)
{
    char * ipv4_address;
    char port[6];
    char regUDP[6]= "59000";
    char regIP[16]= "193.136.138.142";

    /*Check incorrect argument number*/
    if(argc!=3 && argc!=5)
    {
        fprintf(stderr,"Usage: %s <IP> <TCP> <regIP> <regUDP>\n", argv[0]);
        exit(1);
    }

    /*Check arguments format*/

    if(check_port(port)==0 || check_ipv4(ipv4_address)==0) /*estas funções vão ter de ser alteradas*/
    {
        exit(EXIT_FAILURE);
    }

    /*Collect from command line*/
    strcpy(ipv4_address,argv[1]);
    strcpy(port,argv[2]);
    if(argc==5)
    {
       strcpy(regIP,argv[3]);
       strcpy(regUDP,argv[4]); 
    }
    
    fd_set readfds;
    FD_ZERO(&readfds);
    int fdmax; /*ver onde é que vou buscar o fdmax*/


    /*Chamada do select*/
    while(1)
    {
        fd_set tmpfds;
        FD_ZERO(&tmpfds);
        tmpfds=readfds;

        if(select(fdmax+1,&tmpfds,NULL,NULL,NULL)<0)
        {
            printf("Select() Error");
        }

        for(int i=0; i<=fdmax;i++)
        {

        }

    }

}
