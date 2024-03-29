#ifndef __COMMON_H__
#define __COMMON_H__

#define QUEUE_NAME "/async_test_queue4"
#define QUEUE_NAME_TWO "/async_test2_queue4"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

#define CHECK(x)                                                               \
	do {                                                                   \
		if (!(x)) {                                                    \
			fprintf(stderr, "%s:%d: ", __func__, __LINE__);        \
			perror(#x);                                            \
			exit(-1);                                              \
		}                                                              \
	} while (0)

#endif
