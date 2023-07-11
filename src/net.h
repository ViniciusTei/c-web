#ifndef _NET_H_
#define _NET_H_

#include "file.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BACKLOG 10 // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(char *port);
void handle_http_request(int fd, void *cache);
int send_http_response(int fd, int code, char *headers, char *content_type,
                       void *body, int content_len);
void resp_404(int fd);

#endif // !_NET_H_
