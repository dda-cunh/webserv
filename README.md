## Description

Webserv is a project done for [42](https://www.42lisboa.com/), it implements a non-blocking HTTP server that can be configured using a configuration file. It supports GET, POST, and DELETE methods and can serve static files, handle CGI requests, and upload files.

## Requirements

    The program must take a configuration file as an argument or use a default path.
    The server must be non-blocking and use only poll (or equivalent) for all I/O operations.
    The server must be compatible with a web browser of your choice.
    The server must be able to listen to multiple ports.
    The server must be able to serve a fully static website.
    Clients must be able to upload files.
    The server must be able to handle GET, POST, and DELETE methods.

## Configuration File

The configuration file is similar to the server part of the NGINX configuration file. It allows you to specify:

    Port and host for each server
    Server names
    Default error pages
    Client body size limit
    Routes (no regular expressions)
        Accepted HTTP methods
        HTTP redirection
        Directory or file to serve
        Directory listing
        Default file for directories
        CGI execution based on file extension
        Support for POST and GET methods
        Uploading files

webserv does not use any external libraries.

## Getting Started

To compile webserv, you will need a C++98 compiler. You can then compile the program using the following command:

    make

To run webserv, you can use:

    ./webserv [configuration file]

or

    make run CONFIG=[configuration file]


