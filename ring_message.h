#ifndef RING_MESSAGE_H
#define RING_MESSAGE_H

#include "data_structures.h"

void ENTRY (int fd,int new_id,char*new_ip,int new_port);

void SUCC (int fd, int succ_id, char*succ_ip, int succ_port);

void PRED (int fd, node_information *node_info);

int REG (node_information*node_info);

int UNREG (node_information*node_info);

void process_tcp_message(node_information*node_info, char*message, int fd);

int process_new_connection(node_information*node_info, char*message,int fd);

void find_new_max (node_information* node_info);
#endif