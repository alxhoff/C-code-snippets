#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>

#include "common.h"

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

typedef struct mq_info {
	mqd_t fd;
} mq_info_t;

/** Handler for receiving async messages */
void sigHandler(int signal, siginfo_t *info, void *context)
{
	char buffer[MAX_SIZE + 1];
	int must_stop = 0;
	ssize_t bytes_read;
	mq_info_t *mq_i = (mq_info_t *)info->si_value.sival_ptr;
	mqd_t mq = mq_i->fd;

	printf("In handler\n");

	do {
		bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);

		printf("MQ received: %s\n", buffer);
	} while (bytes_read > 0);

	printf("left handler\n");
}

int openMessageQueue(char *name, long max_msg_num, long max_msg_size)
{
	mq_info_t *mq_i = calloc(1, sizeof(mq_info_t));
	struct mq_attr attr;
	struct sigaction sa;
	struct sigevent ev;
	union sigval sv = { .sival_ptr = &mq_i };

	attr.mq_flags = O_NONBLOCK; // Async
	attr.mq_maxmsg = max_msg_num;
	attr.mq_msgsize = max_msg_size;
	attr.mq_curmsgs = 0; // Num of messages currently in queue

	if (-1 == (mq_i->fd = mq_open(name, O_CREAT | O_RDONLY, 0644, &attr)))
		goto error;

	printf("queue opened\n");

	/** Setup handler for SIGIO */
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = sigHandler;
	sigfillset(&sa.sa_mask);
	sigdelset(&sa.sa_mask, SIGIO);
	if (sigaction(SIGIO, &sa, NULL))
		goto error;

	printf("sigaction done\n");

	/** Set up process to be informed about async queue event */
	ev.sigev_notify = SIGEV_SIGNAL; // Specify a signal should be sen
	ev.sigev_value =
		sv; // Suplementary data passed to signal handling fuction
	ev.sigev_notify_function = NULL; // Used by SIGEV_THREAD
	ev.sigev_notify_attributes = NULL; // Used by SIGEV_THREAD

	/** Register this process to receive async notifications when a new message  */
	/**     arrives on the specified message queue  */
	if (mq_notify(mq_i->fd, &ev)) {
		perror("notify failed");
		goto error;
	}

	printf("notify done\n");

	return 0;

error:
	mq_unlink(name);
	return -1;
}

int main(int argc, char *argv[])
{
	openMessageQueue(QUEUE_NAME, 10, MAX_SIZE);

	while (1) {
		printf("waiting...\n");
		sleep(10);
	}

	return 0;
}
