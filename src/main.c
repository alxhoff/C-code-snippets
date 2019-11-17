#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static volatile unsigned char alreadyRegisteredHandler = 0;

typedef struct async_IO_callback {
    int file_desc;
    void (*callback)(int, void*);
    void* args;
    /** struct async_IO_callback; */
} async_IO_callback_t;

async_IO_callback_t cb = { 0 };

void signalHandler(int signal, siginfo_t* sig_info, void* sig_context)
{
    int xSocket;
    if (signal == SIGIO) { // Should be SIG_IO as this was set in registerSignalHandler
        // Signal should be attached to socket file, socket reference can be retrieved from sig_info
        xSocket = sig_info->si_fd;

        if (xSocket == cb.file_desc) {
            if (cb.callback)
                (cb.callback)(xSocket, cb.args);
        }
    }
}

/*
 *  Change the signal handler function for the calling process
 */
void registerSignalHandler(int file_descriptor, int signals)
{
    struct sigaction sa;
    if (!alreadyRegisteredHandler) {
        sa.sa_sigaction = signalHandler;
        sa.sa_flags = SA_SIGINFO;
        sigfillset(&sa.sa_mask);
        sigdelset(&sa.sa_mask, signals);

        // Register signal handler
        if (sigaction(signals, &sa, NULL))
            perror("Could not set signal handler");
        else
            alreadyRegisteredHandler = 1;
    }

    // Make file descriptor generate a signal in current process when an event happens on the file descriptor
    if (fcntl(file_descriptor, F_SETOWN, getpid()))
        fprintf(stderr, "fcntl failed: %d\n", errno);

    // Set file descriptor status flag to show async
    if (fcntl(file_descriptor, F_SETFL, O_ASYNC))
        fprintf(stderr, "fcntl failed: %d\n", errno);
}

void registerIOCallback(int file_descriptor, void (*callback)(int, void*), void* args)
{
    cb.file_desc = file_descriptor;
    cb.callback = callback;
    cb.args = args;

    registerSignalHandler(file_descriptor, SIGIO);
}

int socketOpenUDP(void (*socket_callback)(int, void*), void* callback_args, struct sockaddr_in* socketAddr)
{
    int socket_fd = 0;

    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        printf("Socket not created\n");
        return 1;
    }
    if (socket_fd) {
        // Bind socket
        if (bind(socket_fd, (struct sockaddr*)socketAddr, sizeof(struct sockaddr_in)) < 0)
            fprintf(stderr, "Binding socket failed: %s\n", strerror(errno));
        else
            registerIOCallback(socket_fd, socket_callback, callback_args);
    } else
        fprintf(stderr, "failed to open socket\n");
}

void socketCallback(int socket, void* args) { printf("hello world\n"); }

int main(int argc, char* argv[])
{
    int socket_fd;
    struct sockaddr_in sa = { 0 };
    struct sockaddr_in receive_addr;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(3333);

    socket_fd = socketOpenUDP(socketCallback, NULL, &receive_addr);
}
