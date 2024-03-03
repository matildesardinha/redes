#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

typedef struct NODE_INFO
{
    int id, ring, port;
    char ip[16];

    int succ_id, succ_port,succ_fd; /*Successor data*/
    char succ_ip[16];

    int s_succ_id, s_succ_port; /*Second Successor data*/
    char s_succ_ip[16];

    int pred_id, pred_fd; /*Predecessor data*/
    

    char udp_ip[16]; /*Node server info*/
    int udp_port; 

    struct addrinfo *udp_server_info;

    int tcp_server_fd; /*TCP server socket*/

    /*faltam ponteiros para as tabelas de encaminhamento*/

}node_information;

#endif

node_information* set_up_node(int id,int tcp_port,char*ip,char*reg_ip,int reg_UDP);