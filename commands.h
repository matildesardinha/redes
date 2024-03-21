#ifndef COMMANDS_H
#define COMMANDS_H

#include "data_structures.h"

/* 
*   process_command()
*   Description: processes commands inserted by the user
*
*   Parameters: *node_info: pointer to all the node information
*               *buffer to store the information received
*
*   Returns: void
*/ 
void process_command(node_information* node_info, char *buffer);

/* 
*   djoin()
*   Description: executes the command "direct join"
*
*   Parameters: *node_info: pointer to all the node information
*               *buffer to store the information received
*                succ_id, succ_ip, succ_port: node info to connect to   
*
*   Returns: void
*/ 
void djoin(node_information * node_info,int id, int succ_id,char* succ_ip, int succ_port);

/* 
*   join()
*   Description: executes the command "join"
*
*   Parameters: *node_info: pointer to all the node information
*                ring: ring id to join
*                id: node id inserted by the user  
*
*   Returns: void
*/ 
void join(node_information *node_info,int ring, int id);

/* 
*   show_topology()
*   Description: executes the command "show_topology", shows node info,
*                succ info, 2nd succ info and pred info   
*
*   Parameters: *node_info: pointer to all the node information  
*
*   Returns: void
*/ 
void show_topology (node_information *node_info);

/* 
*   leave()
*   Description: executes the command "leave"
*                  
*   Parameters: *node_info: pointer to all the node information  
*
*   Returns: void
*/ 
void leave(node_information *node_info);

/* 
*   chord()
*   Description: executes the command "chord"
*                  
*   Parameters: *node_info: pointer to all the node information  
*
*   Returns: void
*/ 
void chord (node_information*node_info);

/* 
*   receive_chord()
*   Description: process a message of type: "CHORD i\n"
*                  
*   Parameters: *node_info: pointer to all the node information
*                fd: file descriptor of the connection created 
*                node: node who sent "CHORD"
*
*   Returns: void
*/ 
void receive_chord(node_information* node_info, int node, int fd);

/* 
*   remove_chord()
*   Description: removes the node chord (each node can only create one chord)
*                  
*   Parameters: *node_info: pointer to all the node information
*   
*   Returns: void
*/ 
void remove_chord(node_information *node_info);

/* 
*   node_left()
*   Description: processes closure of a connection with a node
*                  
*   Parameters: *node_info: pointer to all the node information
*                id: id of the node that closed the connection
*                ring: ring id which the node belongs to
*               
*   Returns: void
*/ 
void node_left(node_information*node_info, int id, int ring);

/* 
*   clear_node()
*   Description: resets all node values
*                  
*   Parameters: *node_info: pointer to all the node information
*   
*   Returns: void
*/ 
void clear_node(node_information*node_info);

/* 
*   exit_f()
*   Description: exits the program
*                  
*   Parameters: *node_info: pointer to all the node information
*   
*   Returns: void
*/ 
void exit_f (node_information*node_info);
#endif