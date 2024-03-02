#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

typedef struct NODE_INFO
{
    int id, ring, port;
    char ip[16];

    int succ_id, succ_port; /*Successor data*/
    char succ_ip[16];

    int s_succ_id, s_succ_port; /*Second Successor data*/
    char s_succ_ip[16];

    int pred_id; /*Predecessor data*/

    struct addrinfo *udp_server_info;

    /*faltam ponteiros para as tabelas de encaminhamento*/

}node_information;

#endif