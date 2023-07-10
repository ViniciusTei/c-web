#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include "net.h"

#define PORT "3420"


/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_http_response(int fd, int code, char *headers, char *content_type, char *body, int content_len) {
    const int max_response_size = 262144;
    char response[max_response_size];

   
    if(content_type == NULL) {
        //set default content type
        strcpy(content_type, "text/plain");
    }

    int response_len = 0;
    strcat(response, "HTTP/1.1 ");

    //todo
    //create a helper to tranlate most http codes
    switch (code) {
        case 200:
            strcat(response, "200 OK\n\0");
        break;
        case 404:
            strcat(response, "404 NOT FOUND\n\0");
        break;
        default:
            strcat(response, "500 INTERNAL ERROR\n\0");
        break;
    }

    char content_len_str[5];

    sprintf(content_len_str, "%d", content_len);
    strcat(response, "Content-Length: ");
    strcat(response, content_len_str);
    strcat(response, "\n");
    strcat(response, "Content-Type: ");
    strcat(response, content_type);
    strcat(response, "\n\n");
    
    response_len = strlen(response);
    
    if (content_len > max_response_size - response_len) {
        perror("Content too arge\n");
        return -1;
    }

    //todo
    //append correct body, for now only supports plain text
   
    strcat(response, body);

    printf("RESPONSE:\n%s", response);

    int rv = send(fd, response, response_len, 0);

    if (rv < 0) {
        perror("Something went wrong with send..");
    }

    return rv;
}

/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, void *cache) {
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return;
    }


    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Read the first two components of the first line of the request 
 
    // If GET, handle the get endpoints

    //    Check if it's /d20 and handle that special case
    //    Otherwise serve the requested file by calling get_file()


    // (Stretch) If POST, handle the post request
    printf("REQUEST:\n %s\n\n", request);
    send_http_response(fd, 404, NULL, "text/plain", "NOT FOUND", sizeof("NOT FOUND"));
}

int main(void) {
    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);

    // This is the main loop that accepts incoming connections and
    // responds to the request. The main parent process
    // then goes back to waiting for new connections.
    
    while(1) {
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
        newfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.

        handle_http_request(newfd, NULL);

        close(newfd);
    }

    return 0;
}
