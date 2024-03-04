#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "check_usage.h"

int check_port (int port)
{
    if (port<40000 || port>65535)
    {
        printf("%d is not a valid port number\n",port);
        return 0;
    }
    return 1;
}

int check_ipv4(char*ip)
{
    struct in_addr dst;
    if(inet_pton(AF_INET, ip,&dst)==0)
    {
        printf("%s is not a valid ipv4 address\n",ip);
        return 0;
    }
    return 1;
}





