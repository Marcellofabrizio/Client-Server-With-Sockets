#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int socket_file_descriptor, port_number, n;
    //struct that holds server information
    struct sockaddr_in server_address; 
    struct hostent *server;

    char buffer[256];
    char ip[16];

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    port_number = atoi(argv[2]);

    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_file_descriptor < 0)
    {
        error("ERROR oppening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
        error("ERROR no such host");

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;

    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number);

    if (connect(socket_file_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
        error("ERROR connecting to server");

    else {
        //prints IP address
        for (int i = 0; server->h_addr_list[i]; ++i) {
	    	server_address.sin_addr = *((struct in_addr*) server->h_addr_list[i]);
	    	inet_ntop(AF_INET, &server_address.sin_addr, ip, sizeof(ip));
    
            printf("Successfully established connection to: %s\n", ip);
	    }
    }

    while (true)
    {
        bzero(buffer, 255);
        printf("Enter your message: \n");
        fgets(buffer, 255, stdin);

        n = write(socket_file_descriptor, buffer, strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");
        
        if(buffer[0] == '#')
            break;

        bzero(buffer, 255);

        n = read(socket_file_descriptor, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");

        printf("Message from server: %s\n", buffer);
    }

    close(socket_file_descriptor);

    return 0;
}