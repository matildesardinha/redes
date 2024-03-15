#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

/*Structure with file descriptors*/
typedef struct CONNECTIONS
{
    int* fd;
    char** ip;
    int* port;

} connections;

typedef struct NODE_INFO
{
    int id, ring, port,maxfd;
    char ip[16];

    int succ_id, succ_fd; /*Successor ID*/

    int s_succ_id; /*Second Successor ID*/
    
    int pred_id,pred_fd; /*Predecessor ID*/

    int udp_port;

    char udp_ip[16];

    struct addrinfo *udp_server_info;

    connections * connection;
    
    fd_set readfds;

    int tcp_server_fd; /*TCP server socket*/

}node_information;



node_information* set_up_node(int id,int tcp_port,char*ip,char*reg_ip,int reg_UDP);

#endif