#ifndef ROUTING_H
#define ROUTING_H

#include "data_structures.h"


void ROUTE (int fd,node_information *node_info, int dest);

void ROUTE_EMPTY(int fd,node_information* node_info, int dest);

void show_routing (node_information *node_info,int dest);

void show_path (node_information *node_info, int dest);

void show_forwarding (node_information *node_info);

int find (int target, int*vector);

int add_dest(node_information* node_info, int dest);

int add_neighbour (node_information* node_info, int new);

void remove_dest(node_information * node_info, int dest);

void remove_neighbour (node_information* node_info, int neighbour);

void send_route_messages (node_information *node_info, int dest);

void send_route_new_connect (int fd, node_information* node_info);

void send_empty_route(node_information *node_info, int dest);

void update_tables_after_remove (int removed, node_information *node_info);

void process_route(node_information *node_info,int node, int dest, char*path);

void process_empty_route(node_information *node_info, int node, int dest);

#endif