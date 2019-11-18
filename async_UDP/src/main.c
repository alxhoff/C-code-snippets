#define _GNU_SOURCE // F_SETSIG
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 2000

static volatile unsigned char alreadyRegisteredHandler = 0;

typedef struct async_IO_callback {
	int file_desc;
	void (*callback)(int, void *);
	void *args;
	/** struct async_IO_callback; */
} async_IO_callback_t;

async_IO_callback_t cb = { 0 };

void signalHandler(int signal, siginfo_t *sig_info, void *sig_context)
{
	int xSocket;
	if (signal ==
	    SIGIO) { // Should be SIG_IO as this was set in registerSignalHandler
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
void registerSignalHandler(int file_descriptor)
{
	struct sigaction sa;
	int file_status;

	/** Get file status */
	file_status = fcntl(file_descriptor, F_GETFL);
	file_status |= O_ASYNC | O_NONBLOCK;
	/** Set file descriptor status flag to show async */
	if (fcntl(file_descriptor, F_SETFL, file_status))
		fprintf(stderr, "fcntl failed: %d\n", errno);
	/** Produce a SIGIO signal when i/o is possible */
	fcntl(file_descriptor, F_SETSIG, SIGIO);
	/** Make file descriptor generate a signal in current process when an event happens on the file descriptor */
	if (fcntl(file_descriptor, F_SETOWN, getpid()))
		fprintf(stderr, "fcntl failed: %d\n", errno);

	/** Set handler for SIGIO signal */
	sa.sa_sigaction = signalHandler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);
	sigdelset(&sa.sa_mask, SIGIO);

	/** Set callback for SIGIO */
	if (sigaction(SIGIO, &sa, NULL))
		perror("Could not set signal handler");
}

void registerIOCallback(int socket_fd, void (*callback)(int, void *),
			void *args)
{
	cb.file_desc = socket_fd;
	cb.callback = callback;
	cb.args = args;

	registerSignalHandler(socket_fd);
}

int socketOpenUDP(void (*socket_callback)(int, void *), void *callback_args,
		  struct sockaddr_in *socket_addr)
{
	int socket_fd = 0;

	/** Create socket */
	if ((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		printf("Socket not created\n");
		return 1;
	}

	/** Register callback */
	registerIOCallback(socket_fd, socket_callback, callback_args);

	// Bind socket
	if (bind(socket_fd, (struct sockaddr *)socket_addr,
		 sizeof(*socket_addr))) {
		fprintf(stderr, "Binding socket failed: %s\n", strerror(errno));
		return -1;
	}
	return socket_fd;
}

void socketCallback(int socket, void *args)
{
	int read_size;
	struct sockaddr_in client;
	char client_message[BUFFER_SIZE];
	socklen_t client_sz = sizeof(struct sockaddr_in);

	while ((read_size = recvfrom(socket, client_message, 2000, 0,
				     (struct sockaddr *)&client,
				     (socklen_t *)&client_sz)) > 0) {
		printf("Client message: %s", client_message);
		printf("From %s:%d\n", inet_ntoa(client.sin_addr),
		       ntohs(client.sin_port));
		memset(client_message, 0, strlen(client_message));
	}

	if (read_size == 0)
		printf("Client disconnected\n");
	else if (read_size == -1)
		printf("Transmission finished\n");
}

int main(int argc, char *argv[])
{
	int socket_fd;
	struct sockaddr_in sa = { 0 };

	/** Set socket address */
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(3333);

	if ((socket_fd = socketOpenUDP(socketCallback, NULL, &sa)) < 0)
		printf("Creating registered socket failed\n");

	while (1)
		sleep(1);
}
