#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_LENGTH 2000

int main(int argc, char* argv[])
{
    int socket_file_descriptor;

    // Address structures, port + ip
    struct sockaddr_in server;
    char client_message[BUFFER_LENGTH - 1], server_message[BUFFER_LENGTH];

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
    /** server.sin_addr.s_addr = INADDR_LOOPBACK; // Target address is loopback */
    // Same as
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(1234);

    // Connect to server
    if (connect(socket_file_descriptor, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        return 1;
    }

    printf("Connected\n");

    while (1) {
        printf("$> ");
        /** memset(client_message, 0, strlen(client_message)); */
        scanf("%s", client_message);

        // Send data
        if (send(socket_file_descriptor, client_message, strlen(client_message), 0) < 0) {
            perror("Send failed");
            return 1;
        }
        printf("Client message: %s\n", client_message);

        // Get reply from server
        if (recv(socket_file_descriptor, server_message, BUFFER_LENGTH, 0) < 0) {
            perror("Receive failed");
            break;
        }

        printf("Server replied: %s\n", server_message);
        memset(server_message, 0, strlen(server_message));
    }

    close(socket_file_descriptor);

    return 0;
}
