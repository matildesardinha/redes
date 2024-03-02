#ifndef TCP_FUNCTIONS_H
#define TCP_FUNCTIONS_H

int tcp_server(char *ip, int port);

int tcp_client (char *ip, int port);

ssize_t send_tcp_message(int fd, char*message, int message_size);

ssize_t receive_tcp_message(int fd, char* buffer, int buffer_size);

#endif