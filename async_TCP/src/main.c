#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_LENGTH 2000

#define SERVER_ADDR INADDR_ANY
#define SERVER_PORT 3333

static void action_handler(int sig, siginfo_t* siginfo, void* context)
{
    int read_size, client_fd, server_socket = siginfo->si_fd;
    struct sockaddr_in client;
    socklen_t client_sz = sizeof(struct sockaddr_in);
    char client_message[BUFFER_LENGTH];

    /** si_fd should be storing the file descriptor associated with the server's socket */
    if ((client_fd = accept(server_socket, (struct sockaddr*)&client, &client_sz)) < 0) {
        fprintf(stderr, "Failed to accept: %s\n", strerror(errno));
        return;
    }

    printf("Connection establisted with signal: %d from PID: %ld, UID: %ld\n", sig, (long)siginfo->si_pid, (long)siginfo->si_uid);

    /** Get messages from connected client */
    while ((read_size = recv(client_fd, client_message, BUFFER_LENGTH, 0)) > 0) {
        printf("Client message: %s\n", client_message);
        memset(client_message, 0, strlen(client_message));
    }

    if (!read_size)
        printf("Client disconnected\n");
    else if (read_size < 0)
        fprintf(stderr, "Recv failed: %s\n", strerror(errno));

    close(client_fd);
}

int main(int argc, char* argv[])
{
    int socket_fd, client_fd, file_status;
    struct sockaddr_in server_self;
    struct sigaction act = { 0 };

    /** Create socket */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); // def protocol

    if (socket_fd < 0) {
        perror("Creating socket failed");
        return 1;
    }

    /** Set socket address */
    server_self.sin_family = AF_INET;
    server_self.sin_addr.s_addr = SERVER_ADDR;
    server_self.sin_port = htons(SERVER_PORT);

    /** Bind socket */
    if (bind(socket_fd, (struct sockaddr*)&server_self, sizeof(server_self)) < 0) {
        fprintf(stderr, "Binding failed: %s\n", strerror(errno));
        goto error;
    }

    /** Set sigaction to use sigaction callback and not handler */
    act.sa_flags = SA_SIGINFO;         // Enables sigaction instead of handler
    act.sa_sigaction = action_handler; // Callback function

    /** Set mask to ignore all signals except SIGIO during execution of handler */
    sigfillset(&act.sa_mask);       // Mask all
    sigdelset(&act.sa_mask, SIGIO); // Clear SIGIO from mask

    /** Set callback for SIGIO */
    if (sigaction(SIGIO, &act, NULL) < 0) {
        fprintf(stderr, "Setting sigaction failed: %s\n", strerror(errno));
        goto error;
    }

    /** Set socket io signals to async and make sure SIGIO is sent to current
     * process when a TCP connection is made on the socket
     * */
    file_status = fcntl(socket_fd, F_GETFL); // Get current status
    file_status |= O_ASYNC | O_NONBLOCK;
    fcntl(socket_fd, F_SETFL, file_status); // Set modified status flags
    fcntl(socket_fd, F_SETSIG, SIGIO);      // Produce a SIGIO signal when i/o is possible (connection made)
    fcntl(socket_fd, F_SETOWN, getpid());   // Make sure SIGIO signal is sent to current process

    /** Set socket to listen, connections will trigger callback in which they can be accepted */
    if (listen(socket_fd, 1) < 0) {
        fprintf(stderr, "Failed to listen: %s\n", strerror(errno));
        goto error;
    }

    while (1)
        sleep(10);

    return 0;

error:
    close(socket_fd);
    return 1;
}
