#ifndef RING_MESSAGE_H
#define RING_MESSAGE_H

#include "data_structures.h"

/* 
*   ENTRY()
*   Description: sends message of type ENTRY to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               new_id, new_ip,new_port: parameters sent in the message
*
*   Returns: void
*/ 
void ENTRY (int fd,int new_id,char*new_ip,int new_port);

/* 
*   SUCC()
*   Description: sends message of type SUCC to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               succ_id, succ_ip,succ_port: parameters sent in the message
*
*   Returns: void
*/ 
void SUCC (int fd, int succ_id, char*succ_ip, int succ_port);

/* 
*   PRED()
*   Description: sends message of type PRED to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               *node_info: pointer to all the node information
*
*   Returns: void
*/ 
void PRED (int fd, node_information *node_info);

/* 
*   CHORD()
*   Description: sends message of type CHORD to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               *node_info: pointer to all the node information
*
*   Returns: void
*/ 
void CHORD (int fd, node_information* node_info);

/* 
*   REG()
*   Description: sends message of type REG to the node server
*                  
*   Parameters: *node_info: pointer to all the node information
*
*   Returns: 1 if registation was successful,0 if not
*/ 
int REG (node_information*node_info);

/* 
*   UNREG()
*   Description: sends message of type UNREG to the node server
*                  
*   Parameters: *node_info: pointer to all the node information
*
*   Returns: 1 if unregistation was successful,0 if not
*/ 
int UNREG (node_information*node_info);

/* 
*   process_tcp_message()
*   Description: processes messages from established connections
*                  
*   Parameters: *node_info: pointer to all the node information
*               *message: message received
*                fd: file descriptor where the message came from
*
*   Returns: void
*/ 
void process_tcp_message(node_information*node_info, char*message, int fd);

/* 
*   process_new_connection()
*   Description: processes messages from new connections
*                  
*   Parameters: *node_info: pointer to all the node information
*               *message: message received
*                fd: file descriptor where the message came from
*
*   Returns: void
*/ 
void process_new_connection(node_information*node_info, char*message,int fd);

/* 
*   find_new_max()
*   Description: finds new max fd every time the current maxfd 
*                is no longer the maxfd
*  
*   Parameters: *node_info: pointer to all the node information
*
*   Returns: void
*/ 
void find_new_max (node_information* node_info);
#endif