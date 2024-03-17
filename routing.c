#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "routing.h"
#include "tcp_functions.h"
#include "data_structures.h"

#define BUFFER_SIZE 200
#define NODES 16
#define CONECT 100

void ROUTE (int fd, node_information *node_info, int dest)
{
    char buffer[BUFFER_SIZE];
    int i;
    i=find(dest,node_info->destinations);

    sprintf(buffer, "ROUTE %02d %02d %s\n",node_info->id,dest,node_info->short_way[i]->field);

    send_tcp_message(fd,buffer,strlen(buffer));   
    return;
}

void ROUTE_EMPTY(int fd,node_information* node_info, int dest)
{
    char buffer[BUFFER_SIZE];

    sprintf(buffer, "ROUTE %02d %02d\n", node_info->id,dest);

    send_tcp_message(fd,buffer,strlen(buffer));

    return;
}

void show_routing (node_information *node_info,int dest)
{
    int i=0, j;
    j=find(dest,node_info->destinations);

    printf("-----SHOW ROUTING-----\n");
    printf("PATHS TO %02d\n", dest);

    if( j != -1 && dest != node_info->id)
    {
        for(i=0; i<NODES; i++)
        {
            if( node_info->routing_table[j][i]->n_fields != -1)
            {
                printf("%s\n", node_info->routing_table[j][i]->field);
            }
        }
    }
    else
    {
        printf("There is no path to destination\n");
    }


    printf("---------------------\n");
    return;
}

void show_path (node_information *node_info, int dest)
{
    int i;
    i=find(dest,node_info->destinations);

    printf("-----SHOW PATH-----\n");
    if( i !=-1)
    {
        if( (node_info->short_way[i]->n_fields) != -1)
        {
            printf("PATH TO %02d : %s\n", dest, node_info->short_way[i]->field);
        }
        else
        {
            printf("There is no path to destination\n");
        }
    }
    else
    {
        printf("There is no path to destination\n");
    }

    printf("---------------------\n");
    
    return;
}

void show_forwarding (node_information *node_info)
{
    int i;

    printf("-----SHOW FOWARDING-----\n");
    printf("dest | way\n");

    for(i=0; i<NODES; i++)
    {
        if ( (node_info->expedition[i]) != -1)
        {
            printf("%d | %d\n",node_info->destinations[i],node_info->expedition[i]);
        }  
    }
    printf("---------------------\n");

    return;
}

int find (int target, int*vector)
{
    int i=0;

    while ((vector[i]) != target)
    {
        i++;
        if(i == NODES)
        {
            printf("find() : Could not find target\n");
            i=-1;
            break;
        }
    }

    return i;
}

int add_dest(node_information* node_info, int dest)
{
    int i=0;

    while(node_info->destinations[i] != -1)
    {
        i++;
    }
    node_info->destinations[i]=dest;

    return i;
}

int add_neighbour (node_information* node_info, int new)
{
    int i=0;

    while(node_info->neighbours[i] != -1)
    {
        i++;
    }
    node_info->neighbours[i]=new;

    return i;
}

void remove_dest(node_information * node_info, int dest)
{
    int i;
    i=find(dest,node_info->destinations);
    node_info->destinations[i]=-1;

    return;
}

void remove_neighbour (node_information* node_info, int neighbour)
{
    int i;
    i=find(neighbour,node_info->neighbours);
    node_info->neighbours[i]=-1;
    return;
}


void update_tables_after_remove (int removed, node_information *node_info)
{
    int i=0, max=-1, new_path, neigh;

    neigh=find(removed,node_info->neighbours);
    node_info->neighbours[neigh]=-1;


    /*Remove from routes table*/
    for(i=0; i<NODES; i++)
    {
      if( (node_info->routing_table[i][neigh]->n_fields) !=-1)
      {
        /*Checks if it was the shortest way to a dest*/
        if(strcmp(node_info->routing_table[i][neigh]->field,
                  node_info->short_way[i]->field)==0)
        {
            /*Cleans entrance*/
            sprintf(node_info->routing_table[i][neigh]->field,"-");
            node_info->routing_table[i][neigh]->n_fields=-1;

            /*Updates short_way table*/
            for(int j=0;j<NODES; j++)
            {
                if(max < node_info->routing_table[i][j]->n_fields)
                {
                    max=node_info->routing_table[i][j]->n_fields;
                    new_path=j;
                }
            }

            /*Case: there is no path to dest*/
            if(max == -1)
            {
                node_info->short_way[i]->n_fields=-1;
                sprintf(node_info->short_way[i]->field,"-");
                node_info->expedition[i]=-1;

                /*Remove destination*/
                remove_dest(node_info,node_info->destinations[i]);

                /*Sends empty ROUTE*/
                send_empty_route(node_info,node_info->destinations[i]);

            }
            /*Case: there is a new path to dest*/
            else
            {
                node_info->short_way[i]->n_fields=max;
                strcpy(node_info->short_way[i]->field,node_info->routing_table[i][new_path]->field);
                node_info->expedition[i]= node_info->neighbours[new_path];

                /*Send update to all neighbours*/
                send_route_messages(node_info,node_info->destinations[i]);
            }
        }
        else
        {
            /*Cleans entrance*/
            sprintf(node_info->routing_table[i][neigh]->field,"-");
            node_info->routing_table[i][neigh]->n_fields=-1;

        }
      }
        
    }
    return;
    
}

void send_route_new_connect (int fd, node_information* node_info)
{
    int i=0;

    for(i=0; i<NODES; i++)
    {
        if( (node_info->destinations[i]) !=-1)
        {
                ROUTE(fd,node_info,node_info->destinations[i]);        
        }
    }

    return;
}

void send_route_messages (node_information *node_info, int dest)
{
    int i=0;

    /*Sends route to pred and succ*/
    if(node_info->pred_fd !=-1)
    {
        ROUTE(node_info->pred_fd,node_info,dest);
    }

    if(node_info->succ_fd !=-1)
    {
        ROUTE(node_info->succ_fd,node_info,dest);
    }
    
    /*Sends routes to chords*/
    for(i=0; i<NODES; i++)
    {
        if(node_info->fd[i] != -1)
        {
            ROUTE(node_info->fd[i],node_info,dest);
        }
    }

    return;
}

void send_empty_route(node_information *node_info, int dest)
{
        int i=0;

    /*Sends route to pred and succ*/
    if(node_info->pred_fd !=-1)
    {
        ROUTE_EMPTY(node_info->pred_fd,node_info,dest);
    }

    if(node_info->succ_fd !=-1)
    {
        ROUTE_EMPTY(node_info->succ_fd,node_info,dest);
    }
    
    /*Sends routes to chords*/
    for(i=0; i<NODES; i++)
    {
        if(node_info->fd[i] != -1)
        {
            ROUTE_EMPTY(node_info->fd[i],node_info,dest);
        }
    }

    return;
}

void process_route(node_information *node_info,int node, int dest, char*path)
{
    printf("process_route(%d %d %s)\n",node,dest,path);

    char *token;
    int valid_route=1;
    char final_path[BUFFER_SIZE], path_copy[BUFFER_SIZE];
    int i=0, dest_place, neigh_place;
    char id [3];

    sprintf(id,"%d", node_info->id);

    strcpy(path_copy,path);

    token = strtok(path_copy, "-");
    while (token != NULL) {
        if (!strcmp(token, id)) {
            valid_route = 0;
            printf("route is invalid\n");
            break;
        }
    i++; // Counting the number of tokens
    token = strtok(NULL, "-");
    }

    /*If the route is valid (doesn't include "ourselves")*/
    if (valid_route == 1)
    {
        printf("route is valid\n");

        dest_place=find(dest,node_info->destinations);
        neigh_place=find(node,node_info->neighbours);

        /*If neighbour isn't a neigh in the tables, it is added*/
        if(neigh_place == -1)
        {
            neigh_place=add_neighbour(node_info,node);
            printf("neigh wasnt saved\n");
        }

        /*If dest isn't already a dest in the tables, it is added*/
        if(dest_place == -1)
        {
            dest_place=add_dest(node_info,dest);
            printf("dest wasnt saved\n");
        }

        sprintf(final_path, "%02d-%s", node_info->id, path);
        i++; /*Increments length of path by one*/

        /*Updates route table*/
        strcpy(node_info->routing_table[dest_place][neigh_place]->field,final_path);
        node_info->routing_table[dest_place][neigh_place]->n_fields=i;

        printf ("Final path to be saved: %s | length: %d\nSaved in %d %d\n",final_path,i,dest_place,neigh_place);

        /*Updates short ways table and expedition table*/
        if(node_info->short_way[dest_place]->n_fields > i 
            || node_info->short_way[dest_place]->n_fields == -1)
        {
            printf("Updates shortest way table\n");
            node_info->short_way[dest_place]->n_fields=i;
            strcpy(node_info->short_way[dest_place]->field,final_path);
            node_info->expedition[dest_place]=node;

            /*Sends route messages to neighbours*/
            send_route_messages(node_info,dest);
        }

    }
    return;
}

void process_empty_route(node_information *node_info, int node, int dest)
{
    int dest_place, neigh_place, max=-1,i,new_path;
    dest_place=find(dest,node_info->destinations);
    neigh_place=find (node,node_info->neighbours);
    

    if(dest_place != -1 && neigh_place !=-1)
    {
        /*Update routing table*/
        sprintf(node_info->routing_table[dest_place][neigh_place]->field,"-");
        node_info->routing_table[dest_place][neigh_place]->n_fields=-1;   

        /*Verify if it was the shortest way to destination*/
        if(node_info->expedition[dest_place]==node)
        {
            /*Find new shortest path*/
            for(i=0;i<NODES; i++)
            {
                if(max < node_info->routing_table[dest_place][i]->n_fields)
                {
                    max=node_info->routing_table[dest_place][i]->n_fields;
                    new_path=i;
                }
            }

            /*Case: there is no path to dest*/
            if(max == -1)
            {
                node_info->short_way[dest_place]->n_fields=-1;
                sprintf(node_info->short_way[dest_place]->field,"-");
                node_info->expedition[dest_place]=-1;

                /*Remove destination*/
                remove_dest(node_info,node_info->destinations[dest_place]);

                /*Sends empty ROUTE*/
                send_empty_route(node_info,node_info->destinations[dest_place]);

            }
            /*Case: there is a new path to dest*/
            else
            {
                node_info->short_way[dest_place]->n_fields=max;
                strcpy(node_info->short_way[dest_place]->field,node_info->routing_table[dest_place][new_path]->field);
                node_info->expedition[dest_place]= node_info->neighbours[new_path];

                /*Send update to all neighbours*/
                send_route_messages(node_info,node_info->destinations[dest_place]);
            }
        }

        }
        return;
    }
    


