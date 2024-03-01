#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

typedef struct NODE_INFO
{
    int id, succid, succTCP,ring;
    char ip[16], succIP[16];

    struct addrinfo *udp_server_info;

    /*faltam ponteiros para as tabelas de encaminhamento*/

}node_information;

#endif