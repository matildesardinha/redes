#ifndef COMMANDS_H
#define COMMANDS_H

#include "data_structures.h"

void process_command(node_information* node_info, char *buffer);
/*

*/
void djoin(node_information * node_info,int id, int succ_id,char* succ_ip, int succ_port);

/*

*/
void join(node_information *node_info,int ring, int id);

void show_topology (node_information *node_info);

#endif