#ifndef TCP_FUNCTIONS_H
#define TCP_FUNCTIONS_H

/*
    Description: gets the node server info

    Parameters:
    **udp_server_info: server info given by getaddrinfo()
    *ip : node server ip
    *port: node server port

    Returns: void
*/
struct addrinfo* UDPserver_info(struct addrinfo *udp_server_info, char *ip, char *port);


/*
    Description: sends a request to the node server and receives the answer 

    Parameters: 
    *res: addrinfo
    *message: request to the node server
    message_size: request size
    *buffer: answer
    buffer_size:answer size

    Returns: 1 if the information is received, 0 if not
*/
int message_serverUDP (struct addrinfo *res,char *message, size_t message_size,char*buffer,int buffer_size);

#endif