#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#define MAX_DEST 16

/*Entrys of route table*/

typedef struct  PATHSET
{
    int n_fields;
    char field[MAX_DEST];

} pathset;

/*node_information
*
*   Structure with all the node information
*
*/
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

/* 
*   set_up_node()
*   Description: allocates memory and initializes every variable to inicial values
*                  
*   Parameters: tcp_port and tcp_ip of the node
*               reg_ip and reg_UPP : ip and port of the node server
*   
*   Returns: pointer to a node_information structure
*/ 
node_information* set_up_node(int tcp_port,char*tcp_ip,char*reg_ip,int reg_UDP);

/* 
*   free_node()
*   Description: frees all the memmory that was allocated
*                  
*   Parameters: *node_info: pointer to all the node information
*   
*   Returns: void
*/ 
void free_node(node_information* node_info);

#endif