#include "net.h"
#include <string.h>

/**
 * This gets an Internet address, either IPv4 or IPv6
 *
 * Helper function to make printing easier.
 */
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

/**
 * Return the main listening socket
 *
 * Returns -1 or error
 */
int get_listener_socket(char *port) {
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int yes = 1;
  int rv;

  // This block of code looks at the local network interfaces and
  // tries to find some that match our requirements (namely either
  // IPv4 or IPv6 (AF_UNSPEC) and TCP (SOCK_STREAM) and use any IP on
  // this machine (AI_PASSIVE).

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return -1;
  }

  // Once we have a list of potential interfaces, loop through them
  // and try to set up a socket on each. Quit looping the first time
  // we have success.
  for (p = servinfo; p != NULL; p = p->ai_next) {

    // Try to make a socket based on this candidate interface
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      // perror("server: socket");
      continue;
    }

    // SO_REUSEADDR prevents the "address already in use" errors
    // that commonly come up when testing servers.
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      close(sockfd);
      freeaddrinfo(servinfo); // all done with this structure
      return -2;
    }

    // See if we can bind this socket to this local IP address. This
    // associates the file descriptor (the socket descriptor) that
    // we will read and write on with a specific IP address.
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      // perror("server: bind");
      continue;
    }

    // If we got here, we got a bound socket and we're done
    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  // If p is NULL, it means we didn't break out of the loop, above,
  // and we don't have a good socket.
  if (p == NULL) {
    fprintf(stderr, "webserver: failed to find local address\n");
    return -3;
  }

  // Start listening. This is what allows remote computers to connect
  // to this socket/IP.
  if (listen(sockfd, BACKLOG) == -1) {
    // perror("listen");
    close(sockfd);
    return -4;
  }

  return sockfd;
}

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 *
 * Return the value from the send() function.
 */
int send_http_response(int fd, int code, char *headers, char *content_type,
                       void *body, int content_len) {
  const int max_response_size = 262144;
  char response[max_response_size];

  if (content_type == NULL) {
    // set default content type
    content_type = (char *)malloc((strlen("text/plain") + 1) * sizeof(char));
    strcpy(content_type, "text/plain");
  }

  int response_len = 0;
  strcat(response, "HTTP/1.1 ");

  // todo
  // create a helper to tranlate most http codes
  switch (code) {
  case 200:
    strcat(response, "200 OK\n");
    break;
  case 404:
    strcat(response, "404 NOT FOUND\n");
    break;
  default:
    strcat(response, "500 INTERNAL ERROR\n");
    break;
  }

  strcat(response, "X-Powered-By: GCC\n");

  // Date: <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
  time_t t = time(NULL);
  strcat(response, "Date: ");
  strcat(response, asctime(localtime(&t)));

  char content_len_str[5];

  strcat(response, "Connection: close\n");
  // Representation headers
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

  strcat(response, body);
  strcat(response, "\0");

  response_len = strlen(response);

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

  char method[5], uri[126], *line;

  line = strtok(request, " ");
  strcpy(method, line);
  line = strtok(NULL, line);

  strcpy(uri, line);

  printf("%s %s\n", method, uri);
  resp_404(fd);
}

void resp_404(int fd) {
  char filepath[4096];
  struct file_data *filedata;
  char *mime_type;

  snprintf(filepath, sizeof filepath, "./src/files/404.html");

  filedata = file_load(filepath);

  if (filedata == NULL) {
    char *body;
    body = (char *)malloc((strlen("NOT FOUND") + 1) * sizeof(char));
    strcpy(body, "NOT FOUND");
    send_http_response(fd, 404, NULL, NULL, body, strlen(body));
    return;
  }

  send_http_response(fd, 404, NULL, NULL, filedata->data, filedata->size);
}
