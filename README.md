# Web server

A Web server implementation in C.

What is going to be in this project:

- Networking with sockets
- HTTP request parser and response
- Serve html files
- LRU cache

## What is a Web Server?

A web server is a piece of software that accepts HTTP requests (e.g. GET requests for HTML pages), and returns responses (e.g. HTML pages). Other common uses are GET requests for getting data from RESTful API endpoints, images within web pages, and POST requests to upload data to the server (e.g. a form submission or file upload).

# Networking

## Sockets

Under Unix-like operating systems, the sockets API is the one used to send Internet traffic. It supports both the TCP and UDP protocols, and IPv4 and IPv6.

The sockets API gives access to the IP and TCP layers in the diagram above.

You can create a new socket (socket descriptor) with the socket() system call.

Once created you still have to bind it to a particular IP address (which the OS associates with a particular network card). This is done with the bind() system call.

Once bound, you can read and write data to the socket using the recv() and send() system calls.

## HTTP

_In the webserver, you will be writing code that parses down strings that hold HTTP requests, and builds strings that hold HTTP responses. Study what an HTTP request and response look like._

The final piece of information needed for web traffic is the _HyperText Transport Protocol_ (HTTP). While TCP deals with general data integrity and IP deals with routine, HTTP is concerned with `GET` and `POST` requests of web data.

Like the other stages of networking, HTTP adds a header before the data it wants to send with the packet. Like IP and TCP, this header has a well-defined specification for exactly what needs to be sent.

Though the specification is complex, fortunately only a small amount of information is needed to implement a barebones version.

For each _HTTP request_ from a client, the server sends back an _HTTP response_.

Here is an example HTTP `GET` request and response using version 1.1 of the HTTP protocol getting the page `http://lambdaschool.com/example`:

```
GET /example HTTP/1.1
Host: lambdaschool.com

```

And here is a sample HTTP response:

```
HTTP/1.1 200 OK
Date: Wed Dec 20 13:05:11 PST 2017
Connection: close
Content-Length: 41749
Content-Type: text/html

<!DOCTYPE html><html><head><title>Lambda School ...
```

The end of the header on both the request and response is marked by a blank line (i.e. two newlines in a row).

If the file is not found, a `404` response is generated and returned by the server:

```
HTTP/1.1 404 NOT FOUND
Date: Wed Dec 20 13:05:11 PST 2017
Connection: close
Content-Length: 13
Content-Type: text/plain

404 Not Found
```

If you've ever looked in the Network panel of your web browser's debugger, some of these headers might look familiar.

Important things to note:

* For HTTP/1.1, the request **must** include the `Host` header.
* The second word of the first line of the response gives you a success or failure indicator.
* `Content-Length` gives the length of the request or response body, not counting the blank line between the header and the body.
* `Content-Type` gives you the MIME type of the content in the body. This is how your web browser knows to display a page as plain text, as HTML, as a GIF image, or anything else. They all have their own MIME types.
* Even if your request has no body, a blank line still **must** appear after the header.
* `Connection: close` tells the web browser that the TCP connection will be closed after this response. This should be included.
* The `Date` should be the date right now, but this field is optional.
