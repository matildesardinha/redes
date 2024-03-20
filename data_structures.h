#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#define MAX_DEST 16

/*Entrys of route table*/

typedef struct  PATHSET
{
    int n_fields;
    char field[MAX_DEST];

} pathset;


typedef struct NODE_INFO
{
    int id, ring,maxfd, tcp_port;

    char tcp_ip[16];

    int succ_id; /*Successor ID*/
    int succ_fd;

    int s_succ_id; /*Second Successor ID*/
    
    int pred_id; /*Predecessor ID*/
    int pred_fd;

    int chord_id;

    int *fd;
    char **ip;
    int *port;

    int udp_port;
    char udp_ip[16];
    struct addrinfo *udp_server_info;
    
    fd_set readfds;

    int tcp_server_fd; /*TCP server socket*/

    /*Expedition table, routing table, shortest ways table*/
    int *expedition;
    pathset ** short_way;
    pathset *** routing_table;

    int *destinations; /*Destinations vector*/
    int * neighbours; /*Neighbours vector*/


}node_information;


node_information* set_up_node(int tcp_port,char*tcp_ip,char*reg_ip,int reg_UDP);


#endif