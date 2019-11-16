#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#include "common.h"

//https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system

int main(int argc, char *argv[])
{
	mqd_t mq;
	struct mq_attr attr;
	char buffer[MAX_SIZE + 1];
	int must_stop = 0;

	attr.mq_flags = 0; // Flags, 0 of O_NONBLOCK
	attr.mq_maxmsg = 10; // Max num of messages in queue
	attr.mq_msgsize = MAX_SIZE; // Max size of message in bytes
	attr.mq_curmsgs = 0; // Num of messages currently in queue

	mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
	CHECK((mqd_t)-1 != mq);

	printf("Send to server (enter \"exit\" to stop):\n");

	do {
		ssize_t bytes_read;

		bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
		CHECK(bytes_read >= 0);

		buffer[bytes_read] = '\0';
		if (!strncmp(buffer, MSG_STOP, strlen(MSG_STOP))) {
			must_stop = 1;
		} else {
			printf("Received: %s\n", buffer);
		}

	} while (!must_stop);

	CHECK((mqd_t)-1 != mq_close(mq));
	CHECK((mqd_t)-1 != mq_unlink(QUEUE_NAME));

	return 0;
}
