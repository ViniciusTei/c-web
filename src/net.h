#ifndef _NET_H_
#define _NET_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define BACKLOG 10	 // how many pending connections queue will hold

void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(char *port);

#endif // !_NET_H_
#define _NET_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>



void *get_in_addr(struct sockaddr *sa);
int get_listener_socket(char *port);
