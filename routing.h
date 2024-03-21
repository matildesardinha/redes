#ifndef ROUTING_H
#define ROUTING_H

#include "data_structures.h"

/* 
*   ROUTE()
*   Description: sends message of type ROUTE to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               dest: destination the route message is about
*
*   Returns: void
*/ 
void ROUTE (int fd,node_information *node_info, int dest);

/* 
*   ROUTE_EMPTY()
*   Description: sends message of type ROUTE to a certain node,
*                this message doesn't send a path since it doesn't exist   
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               dest: destination the route message is about
*
*   Returns: void
*/ 
void ROUTE_EMPTY(int fd,node_information* node_info, int dest);

/* 
*   CHAT()
*   Description: sends message of type  to a certain node
*                  
*   Parameters: fd: file descriptor to be used when sending the message
*               node, dest, message: chat content
*
*   Returns: void
*/ 
void CHAT (int fd, int node, int dest, char*message);

/* 
*   show_routing()
*   Description: shows routes to a certain node
*                  
*   Parameters: *node_info :  pointer to all the node information
*                dest : shows routing for this dest
*
*   Returns: void
*/ 
void show_routing (node_information *node_info,int dest);

/* 
*   show_path()
*   Description: shows route to a certain node
*                  
*   Parameters: *node_info :  pointer to all the node information
*                dest : shows routing for this dest
*
*   Returns: void
*/ 
void show_path (node_information *node_info, int dest);

/* 
*   show_fowarding()
*   Description: shows fowarding to a certain node
*                  
*   Parameters: *node_info :  pointer to all the node information
*
*   Returns: void
*/ 
void show_forwarding (node_information *node_info);

/* 
*   find()
*   Description:finds location of a target in a vector
*                  
*   Parameters: *vector : array to search in
*                target: int to find in the array
*
*   Returns: -1 if doesn't find the target, if it does find it
*            returns the location value   
*/ 
int find (int target, int*vector);

/* 
*   add_dest()
*   Description: adds a destination to the destination array
*                  
*   Parameters: *node_info :  pointer to all the node information
*                dest : dest to be added
*
*   Returns: place where dest was stored
*/ 
int add_dest(node_information* node_info, int dest);

/* 
*   add_neighbour()
*   Description: adds a destination to the destination array
*                  
*   Parameters: *node_info :  pointer to all the node information
*                new : neighbour to be added
*
*   Returns: place where new was stored
*/
int add_neighbour (node_information* node_info, int new);

/* 
*   remove_dest()
*   Description: removes a destination from destination array
*                  
*   Parameters: *node_info :  pointer to all the node information
*                dest: destination to be removed
*
*   Returns: void
*/
void remove_dest(node_information * node_info, int dest);

/* 
*   remove_neighbour()
*   Description: removes a neighbour from neighbour array
*                  
*   Parameters: *node_info :  pointer to all the node information
*                neighbour: neighbour to be removed
*
*   Returns: void
*/
void remove_neighbour (node_information* node_info, int neighbour);

/* 
*   send_route_messages()
*   Description: sends route messages to all neighbours after an entry
*                of the shortest paths table is altered
*                  
*   Parameters: *node_info :  pointer to all the node information
*                dest: destination to where there is a new shortest path
*
*   Returns: void
*/
void send_route_messages (node_information *node_info, int dest);

/* 
*   send_route_new_connect()
*   Description: sends route messages to a new connection. Full shortest path
*                table is sent
*  
*   Parameters: *node_info :  pointer to all the node information
*               fd: file descriptor of the new connection
*
*   Returns: void
*/
void send_route_new_connect (int fd, node_information* node_info);

/* 
*   send_empty_route()
*   Description: sends "empty" when the is no longer a path to a dest.
*                this message is sent to all neighbours   
*  
*   Parameters: *node_info :  pointer to all the node information
*                dest: destination to where there is no longer a path to
*
*   Returns: void
*/
void send_empty_route(node_information *node_info, int dest);

/* 
*   update_tables_after_remove()
*   Description: after a connection is removed, all the tables need to be updated
*                  
*   Parameters: *node_info :  pointer to all the node information
*                removed: id of the node who broke the connection
*
*   Returns: void
*/
void update_tables_after_remove (int removed, node_information *node_info);

/* 
*   process_route()
*   Description: updates tables after receiving a ROUTE message
*                  
*   Parameters: *node_info :  pointer to all the node information
*                node, dest, path : parameters of ROUTE message
*
*   Returns: void
*/
void process_route(node_information *node_info,int node, int dest, char*path);

/* 
*   process_empty_route()
*   Description: updates tables after receiving a ROUTE message
*                  
*   Parameters: *node_info :  pointer to all the node information
*                node, dest, path : parameters of ROUTE message
*
*   Returns: void
*/
void process_empty_route(node_information *node_info, int node, int dest);

/* 
*   send_chat()
*   Description: sends a chat message
*                  
*   Parameters: *node_info :  pointer to all the node information
*                node, dest, path : parameters of CHAT message
*
*   Returns: void
*/
void send_chat(node_information*node_info, int node, int dest, char*message);

/* 
*   receive_chat()
*   Description: receives a chat message and sends it to another node, 
*                if the message is not for itself   
*                  
*   Parameters: *node_info :  pointer to all the node information
*                node, dest, path : parameters of ROUTE message
*
*   Returns: void
*/
void receive_chat(node_information* node_info, int node, int dest, char*message);

#endif