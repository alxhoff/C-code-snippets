#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>

#include "common.h"

int main(int argc, char *argv[])
{
	mqd_t mq, mq2;
	char buffer[MAX_SIZE];

	mq = mq_open(QUEUE_NAME, O_WRONLY);
	CHECK((mqd_t)-1 != mq);

	mq2 = mq_open(QUEUE_NAME, O_WRONLY);
	CHECK((mqd_t)-1 != mq);

	printf("Send to server (enter \"exit\" to stop)\n");

	do {
		printf("$> ");
		fflush(stdout);

		memset(buffer, 0, MAX_SIZE);
		fgets(buffer, MAX_SIZE, stdin);

		CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
		CHECK(0 <= mq_send(mq2, buffer, MAX_SIZE, 0));
	} while (strncmp(buffer, MSG_STOP, strlen(MSG_STOP)));

	CHECK((mqd_t)-1 != mq_close(mq));
	CHECK((mqd_t)-1 != mq_close(mq2));

	return 0;
}
