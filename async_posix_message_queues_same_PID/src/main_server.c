
#define _GNU_SOURCE

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <mqueue.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>

//https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system

/** Struct to keep track of which message queue is generating signal. */
/** When a mq generated a signal and sigevent was set to SIGEV_SIGNAL,  */
/** the si_value attribute in the siginfo_t struct (passed to sigaction handler) */
/** is set to whatever the value was in sigev_value. Therefore storing a pointer */
/** to a structure containing mq information in sigev_value will allow for the mq  */
/** to be known from within the signal handler.  */
/**  */
/** Setup: create mq_info_t struct and then set sigevent.sigev_value.sigval_prt  */
/** to point to structure.  */
/**  */
/** In handler: The siginfo_t structure will hold the pointer to the mq_info_t struct  */
/** via siginfo_t->si_value.sivalue  */

#define QUEUE_NAME_ONE "async_test_queue"
#define QUEUE_NAME_TWO QUEUE_NAME_ONE "_2"
#define MAX_SIZE 1024

typedef struct mq_info {
	mqd_t fd;
	void (*callback)(ssize_t, char *, void *);
	void *args;
	char *name;
	char *buffer;
	struct mq_info *next;
	struct sigevent ev;
} mq_info_t;

mq_info_t head = { 0 };

void closeMsgQueues(int signal)
{
	if (signal == SIGINT) {
		printf("Closing queues\n");
		mq_info_t *iterator = &head, *delete;

		while (iterator->next) {
			iterator = iterator->next;

			delete = iterator;
			printf("Closing message queue: %d, name: %s\n",
			       delete->fd, delete->name);
			/** Close message queue  */
			mq_close(delete->fd);
			/** Remove message queue */
			mq_unlink(delete->name);
			free(delete->name);
			free(delete->buffer);
			free(delete);
			printf("Closed\n");
		}

		exit(EXIT_SUCCESS);
	}
}

/** Handler for receiving async messages */
void sigHandler(int signal, siginfo_t *info, void *context)
{
	sigset_t sigs;
	sigemptyset(&sigs);
	pthread_sigmask(0, NULL, &sigs);
	if (sigismember(&sigs, SIGIO)) {
		printf("SIGIO being blocked in handler\n");
		sigaddset(&sigs, SIGIO);
		pthread_sigmask(SIG_UNBLOCK, &sigs, NULL);
	}

	ssize_t bytes_read;
	mq_info_t *mq_i = (mq_info_t *)(info->si_value.sival_ptr);
	printf("Info @ %p\n", mq_i);
	mqd_t mq = mq_i->fd;

	bytes_read = mq_receive(mq, mq_i->buffer, MAX_SIZE, NULL);

	if (bytes_read > 0)
		(mq_i->callback)(bytes_read, mq_i->buffer, mq_i->args);

	/** reregister message queue recieve event */
	if (mq_notify(mq_i->fd, &mq_i->ev) < 0) {
		perror("notify failed");
		closeMsgQueues(SIGINT);
	}
}

int openMessageQueue(char *name, long max_msg_num, long max_msg_size,
		     void (*callback)(ssize_t, char *, void *), void *args)
{
	mq_info_t *iterator;
	struct mq_attr attr;
	struct sigaction sa;
	union sigval sv;
	mq_info_t *conn;

	for (iterator = &head; iterator->next; iterator = iterator->next)
		;

	conn = iterator->next = calloc(1, sizeof(mq_info_t));
	assert(conn);
	printf("Create conn for %s @ %p\n", name, conn);
	conn->name = malloc(sizeof(char) * (strlen(name) + 2));
	assert(conn->name);
	conn->buffer = calloc(MAX_SIZE + 1, sizeof(char));
	assert(conn->buffer);

	strcpy(conn->name + 1, name);
	conn->name[0] = '/';

	conn->callback = callback;
	conn->args = args;

	/** set pointer to MQ strct to be sent to handler */
	sv.sival_ptr = conn;

	/** Attributes of MQ needed for opening MQ */
	attr.mq_maxmsg = max_msg_num;
	attr.mq_msgsize = max_msg_size;
	attr.mq_curmsgs = 0; // Num of messages currently in queue

	assert((conn->fd = mq_open(conn->name, O_CREAT | O_RDONLY | O_NONBLOCK,
				   0644, &attr)));

	/** Setup handler for SIGIO */
	/** sigaction(2) specifies that the triggering signal is blocked in the handler  */
	/**     unless SA_NODEFER is specified */
    sa.sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER; 
	sa.sa_sigaction = sigHandler;
	/** sa_mask specifies signals that will be blocked in the thread the signal  */
	/**     handler executes in */
	sigfillset(&sa.sa_mask);
	sigdelset(&sa.sa_mask, SIGIO);
	if (sigaction(SIGIO, &sa, NULL)) {
		printf("Sigaction failed\n");
		goto error;
	}

	printf("Handler set in PID: %d for TID: %d\n", getpid(), gettid());

	/** fcntl(2) - FN_SETOWN_EX is used to target SIGIO and SIGURG signals to a  */
	/**     particular thread */
	struct f_owner_ex cur_tid = { .type = F_OWNER_TID, .pid = gettid() };
	assert(-1 != fcntl(conn->fd, F_SETOWN_EX, &cur_tid));

	/** Set up process to be informed about async queue event */
	/** If signal handler was registered up using the SA_SIGINFO sigaction flag then  */
	/**     si_value in the siginfo_t struct (passed to handler) is set to the value  */
	/**     of sigev_value. sigev_value is of type sigval which contains sival_ptr, */
	/**     a void pointer than can be set. This pointer is used to pass in a pointer  */
	/**     to the struct containing the message queues properties, including the  */
	/**     required mqd_t. */
	conn->ev.sigev_notify = SIGEV_SIGNAL;
	conn->ev.sigev_signo = SIGIO; // Signal of interest
	conn->ev.sigev_value =
		sv; // Suplementary data passed to signal handling fuction
	conn->ev.sigev_notify_function = NULL; // Used by SIGEV_THREAD
	conn->ev.sigev_notify_attributes = NULL; // Used by SIGEV_THREAD

	/** Register this process to receive async notifications when a new message  */
	/**     arrives on the specified message queue  */
	if (mq_notify(conn->fd, &conn->ev) < 0) {
		perror("notify failed");
		goto error;
	}

	printf("Opened queue '%s'\n", conn->name);

	return 0;

error:
	perror("Open failed");
	mq_unlink(name);
	return -1;
}

int messageQueuePut(char *mq_name, char *buffer)
{
	mqd_t mq;
	int err;
	char *full_name = calloc(strlen(mq_name) + 2, sizeof(char));
	strcpy(full_name + 1, mq_name);
	full_name[0] = '/';

	mq = mq_open(full_name, O_WRONLY);

	free(full_name);

	if ((mqd_t)-1 == mq) {
		printf("Unable to open MQ '%s'\n", mq_name);
		return -1;
	}

	if (-1 == mq_send(mq, buffer, strlen(buffer), 0))
		printf("Unable to send to MQ: %s, errno: %d\n", mq_name, errno);
	else
		printf("Sent to MQ: %s\n", mq_name);

	assert((mqd_t)-1 != mq_close(mq));

	return 0;
}

void initMsgQueues(void)
{
	struct sigaction sa;

	sa.sa_handler = closeMsgQueues;
	sigaction(SIGINT, &sa, NULL);
}

void *sender(void *args)
{
	while (1) {
		printf("**** TICK ****\n");
		messageQueuePut(QUEUE_NAME_ONE, "Hello MQ one");
		messageQueuePut(QUEUE_NAME_TWO, "Hello MQ two");
		sleep(1);
	}
}

void MQCallbackOne(ssize_t read_size, char *buffer, void *args)
{
	if (read_size > 0)
		printf("TID:%d -> In callback ONE: %s\n", gettid(), buffer);
}

void MQCallbackTwo(ssize_t read_size, char *buffer, void *args)
{
	if (read_size > 0)
		printf("TID:%d -> In callback TWO: %s\n", gettid(), buffer);
}

int main(void)
{
	initMsgQueues();

	pthread_t p_sender;

	assert(!openMessageQueue(QUEUE_NAME_ONE, 10, MAX_SIZE, MQCallbackOne,
				 NULL));
	assert(!openMessageQueue(QUEUE_NAME_TWO, 10, MAX_SIZE, MQCallbackTwo,
				 NULL));

	assert(!pthread_create(&p_sender, NULL, sender, NULL));

	while (1) {
		printf("waiting...\n");
		sleep(10);
	}

	return 0;
}
