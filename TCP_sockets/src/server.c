#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LENGTH 2000

int main(int argc, char* argv[])
{
    int socket_file_descriptor, client_socket;

    // Address structures, port + ip
    struct sockaddr_in server, client;
    char client_message[BUFFER_LENGTH];

    // Create socket
    // Communication domain:
    //      AF_INET - IPv4 Internet protocols
    // Commication type (semantics):
    //      SOCK_DGRAM - Datagrams (UDP)
    //      SOCK_STREAM - sequenced (TCP)
    // Protocol:
    //      IPPROTO_UDP
    //      IPPROTO_TCP
    socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_file_descriptor == -1) {
        printf("socket could not be opened\n");
        exit(1);
    } else {
        printf("socket created\n");
    }

    // Prepare server's address structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // Bind socket to all local interfaces
    server.sin_port = htons(1234);

    // Bind socket to port
    if (bind(socket_file_descriptor, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    printf("Binding completed\n");

    // Set socket as listening
    listen(socket_file_descriptor, 3);

    // Accept connections
    socklen_t connection_addr_size = sizeof(struct sockaddr_in);

    // Accept connections to socket descriptor, store the client address information in client and the client's addr size in connection_addr_size
    client_socket = accept(socket_file_descriptor, (struct sockaddr*)&client, (socklen_t*)&connection_addr_size);
    if (client_socket < 0) {
        perror("Accepting failed");
        return 1;
    }

    printf("Connection accepted\n");

    int read_size; // Number of bytes received
    while ((read_size = recv(client_socket, client_message, BUFFER_LENGTH, 0)) > 0) {
        printf("Client message: %s\n", client_message);
        // Echo message
        write(client_socket, client_message, strlen(client_message));
        memset(client_message, 0, strlen(client_message));
    }

    if (read_size == 0)
        printf("Client disconnected\n");
    else if (read_size == -1)
        perror("Recv failed");

    return 0;
}
