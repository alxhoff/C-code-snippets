#define _GNU_SOURCE // F_SETSIG
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define UDP_BUFFER_SIZE 2000
#define CHECK(x)                                                               \
	do {                                                                   \
		if (!(x)) {                                                    \
			fprintf(stderr, "%s:%d: ", __func__, __LINE__);        \
			perror(#x);                                            \
			exit(-1);                                              \
		}                                                              \
	} while (0)

typedef void *aIO_handle_t;

typedef enum {
	NONE = 0,
	TCP,
	UDP,
	MSG_QUEUE,
	SERIAL,
	NO_OF_CONN_TYPES
} aIO_conn_e;

typedef struct {
	int fd;
	struct sockaddr_in addr;
} aIO_socket_t;

typedef struct {
    //TODO
} aIO_mq_t;

typedef struct {
	//TODO
} aIO_serial_t;

typedef union {
	aIO_socket_t socket;
	aIO_mq_t mq;
	aIO_serial_t tty;
} aIO_attr;

typedef struct aIO {
	aIO_conn_e type;

	aIO_attr attr;
	ssize_t buffer_size;
	char *buffer;

	pthread_t thread;

	void (*callback)(ssize_t, char *, void *);
	void *args;
	struct aIO *next;

	pthread_mutex_t lock;
} aIO_t;
typedef struct async_IO_callback {
	int file_desc;
	void (*callback)(int, void *);
	void *args;
	/** struct async_IO_callback; */
} async_IO_callback_t;

aIO_t head = { .type = NONE, .lock = PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t aIO_quit_conn = PTHREAD_COND_INITIALIZER;
pthread_mutex_t aIO_quit_lock = PTHREAD_MUTEX_INITIALIZER;

static int finished = 0;

aIO_t *getLastConnection(void)
{
	aIO_t *iterator;

	for (iterator = &head; iterator->next; iterator = iterator->next)
		;

	return iterator;
}

static aIO_t *findConnection(aIO_conn_e type, void *arg)
{
	aIO_t *prev = &head, *curr;

	pthread_mutex_lock(&prev->lock);
	while ((curr = prev->next) != NULL) {
		pthread_mutex_lock(&curr->lock);
		switch (type) {
		case TCP:
		case UDP:
			if (curr->type == type)
				if (curr->attr.socket.fd == *(int *)arg) {
					pthread_mutex_unlock(&prev->lock);
					pthread_mutex_unlock(&curr->lock);
					return curr;
				}
            break;
		case MSG_QUEUE:
			//TODO
			return NULL;
		case SERIAL:
			//TODO
			return NULL;
		case NONE:
		default:
			return NULL;
		}
		pthread_mutex_unlock(&prev->lock);
		prev = curr;
	}
	pthread_mutex_unlock(&prev->lock);
	return NULL;
}

void aIODeinit(int sig)
{
    aIO_t *iterator, *del;
    finished = 1;

	printf("Stopping listener threads\n");
    pthread_mutex_lock(&aIO_quit_lock);
	pthread_cond_broadcast(&aIO_quit_conn);
    printf("Quit broadcast sent\n");
    pthread_mutex_unlock(&aIO_quit_lock);

    if(head.next)
        for(iterator=head.next; iterator;){
            del = iterator;
            iterator= iterator->next;
            switch(del->type){
            case UDP:
            case TCP:
                printf("Deinit socket %d\n", ntohs(del->attr.socket.addr.sin_port));
                free(del->buffer);
                CHECK(!close(del->attr.socket.fd));
                CHECK(!pthread_cancel(del->thread));
                free(del);
                break;
            default:
                break;
            }
        }

    exit(EXIT_SUCCESS);
}

aIO_t *createAsyncIO(aIO_conn_e type, ssize_t buffer_size,
		     void (*callback)(ssize_t, char *, void *), void *args)
{
	aIO_t *ret = (aIO_t *)calloc(1, sizeof(aIO_t));

	CHECK(ret);

	ret->buffer_size = buffer_size;
	ret->buffer = (char *)malloc(ret->buffer_size * sizeof(char));
	CHECK(ret->buffer);

	ret->type = type;
	ret->callback = callback;
	ret->args = args;

	pthread_mutex_init(&ret->lock, NULL);

	return ret;
}

static void aIOUDPSigHandler(int signal, siginfo_t *info, void *context)
{
	ssize_t read_size;
	int server_fd = info->si_fd;
	aIO_t *conn = findConnection(UDP, &server_fd);
	
    printf("In UDP handler for port %d\n", ntohs(conn->attr.socket.addr.sin_port));

	CHECK(conn);

	pthread_mutex_lock(&conn->lock);

	while ((read_size = recv(server_fd, conn->buffer, conn->buffer_size,
				 0)) > 0) {
		conn->buffer[(read_size - 1) <= conn->buffer_size ?
				     (read_size - 1) :
				     conn->buffer_size] = '\0';
		printf("Buffer: %s\n", conn->buffer);
		(conn->callback)(read_size, conn->buffer, conn->args);
	}

	pthread_mutex_unlock(&conn->lock);
}

void *aIOOpenUDPSocketThread(void *s_udp_fd)
{
	int fd = *(int *)s_udp_fd;
	printf("Created UDP thread with PID: %d\n", gettid());
	CHECK(-1 != fcntl(fd, F_SETOWN, gettid()));

    pthread_mutex_lock(&aIO_quit_lock);
    while(!finished){
        printf("UDP listener on fd %d waiting\n", fd);
        pthread_cond_wait(&aIO_quit_conn, &aIO_quit_lock);
    }
    printf("Closing UDP thread\n");
    pthread_mutex_unlock(&aIO_quit_lock);

    pthread_exit(NULL);
}

aIO_handle_t aIOOpenUDPSocket(char *s_addr, in_port_t port, ssize_t buffer_size,
			      void (*callback)(ssize_t, char *, void *),
			      void *args)
{
	aIO_t *conn = getLastConnection();

	conn->next = createAsyncIO(UDP, buffer_size, callback, args);

	pthread_mutex_lock(&conn->next->lock);

	aIO_socket_t *s_udp = &conn->next->attr.socket;

	s_udp->addr.sin_family = AF_INET;
	s_udp->addr.sin_addr.s_addr = s_addr ? inet_addr(s_addr) : INADDR_ANY;
	s_udp->addr.sin_port = htons(port);

	s_udp->fd = socket(AF_INET, SOCK_DGRAM, 0);
	CHECK(s_udp->fd);

	struct sigaction act = { 0 };
	int fs;

	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = aIOUDPSigHandler;
	sigfillset(&act.sa_mask);
	sigdelset(&act.sa_mask, SIGIO);
	CHECK(!sigaction(SIGIO, &act, NULL));

	CHECK((fs = fcntl(s_udp->fd, F_GETFL)));
	fs |= O_ASYNC | O_NONBLOCK;
	CHECK(-1 != fcntl(s_udp->fd, F_SETFL, fs));
	fcntl(s_udp->fd, F_SETSIG, SIGIO);

    CHECK(!bind(s_udp->fd, (struct sockaddr *)&s_udp->addr,
            sizeof(s_udp->addr)));

    CHECK(!pthread_create(&conn->next->thread, NULL, aIOOpenUDPSocketThread,
                  (void *)&conn->next->attr.socket.fd));

    printf("Thread handle @ -> %p\n", &conn->next->thread);

	pthread_mutex_unlock(&conn->next->lock);

	return (aIO_handle_t)conn->next;
}

void UDPHandler(ssize_t read_size, char *buffer, void *args)
{
	printf("UDP Recv %ld : %s\n", read_size, buffer);
}

int main (void){

    struct sigaction sa = {0};
	char *addr = NULL; // Loopback
	in_port_t port = 1111;

    sa.sa_handler = aIODeinit;
    CHECK(!sigaction(SIGQUIT, &sa, 0));

	aIO_handle_t soc =
		aIOOpenUDPSocket(addr, port, UDP_BUFFER_SIZE, UDPHandler, NULL);

	printf("UDP socket opened on port %d\n", port);
	printf("Demo UDP Socket can be tested using\n");
	printf("*** netcat -vv localhost %d -u ***\n", port);

    port = 2222;

	aIO_handle_t soc_two =
		aIOOpenUDPSocket(addr, port, UDP_BUFFER_SIZE, UDPHandler, NULL);

	printf("UDP socket opened on port %d\n", port);
	printf("Demo UDP Socket can be tested using\n");
	printf("*** netcat -vv localhost %d -u ***\n", port);

	while (1) {
		printf("UDP tick\n");
        sleep(1);
	}
}
