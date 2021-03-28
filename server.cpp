#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{

    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{

    int socket_file_descriptor, port_number, new_socket_file_descriptor;
    int n;
    // A sockaddr_in is a structure containing an internet address.
    socklen_t client_size;
    struct sockaddr_in server_address, client_address;
    struct hostent *server;
    char buffer[256];

    if (argc < 2)
    {
        fprintf(stderr, "ERROR no port provided\n");
        exit(1);
    }

    // AF_INET -> IPv4
    // SOCK_STREAM -> TCP
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_file_descriptor < 0)
    {
        error("ERROR oppening socket");
    }

    /**
     * The function bzero() sets all values in a buffer to zero. 
     * It takes two arguments, the first is a pointer to the buffer
     * and the second is the size of the buffer. Thus, this line 
     * initializes server_address to zeros.
    */
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;

    port_number = atoi(argv[1]);
    /** 
     * Instead of simply copying the port number to this field, 
     * it is necessary to convert this to network byte order using 
     * the function htons() which converts a port number in host byte 
     * order to a port number in network byte order.
    */
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Links the socket to one of the server's TCP ports
    if (bind(socket_file_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        error("ERROR on binding socket to TCP port");

    /** 
     * The listem system call allows the process to listen the 
     * socket port for connection. The first argument is the socket
     * descriptor and the second is the maximum number of connections
     * that can wait whille the process handles one connection
    */

    printf("Listening to port: %d\n", port_number);

    listen(socket_file_descriptor, 50);

    client_size = sizeof(client_address);

    /** 
     * The accept system call blocks the process until a client
     * connects to the server. It wakes up when it receives a 
     * connection from the client. It will return a new file 
     * descriptor for the socket which will be used for the
     * communications of the new connection. Second param is
     * the address of the client and the third is its size
    */
    new_socket_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client_address, &client_size);

    if (new_socket_file_descriptor < 0)
        error("ERROR on acceptiong connection from client");

    while (true)
    {
        bzero(buffer, 256); //clears buffer
        n = read(new_socket_file_descriptor, buffer, 255);
        if (n < 0)
            error("ERROR reading socket");

        if(buffer[0] == '#')
            break;

        printf("Message received from client: %s\n", buffer);

        char new_message[20] = "Hello from server";
        n = write(new_socket_file_descriptor, new_message, 20);
        if (n < 0)
            error("ERROR writing to socket");
    }

    close(new_socket_file_descriptor);
    close(socket_file_descriptor);

    return 0;
}