#ifndef RING_MESSAGE_H
#define RING_MESSAGE_H

void ENTRY (int fd,int new_id,char*new_ip,int new_port);

void SUCC (int fd, int succ_id, char*succ_ip, int succ_port);

void PRED (int fd, node_information *node_info);

int REG (node_information*node_info);
#endif