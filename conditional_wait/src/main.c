
/*
Answer to the following Question
http://stackoverflow.com/questions/29276773/
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *function1();
void *function2();
void *function3();
void *function4();

/* Global Variable */
int count = 1;

int main(void)
{
	pthread_t thread1, thread2, thread3, thread4;
	pthread_create(&thread4, NULL, function4, NULL);
	pthread_create(&thread3, NULL, function3, NULL);
	pthread_create(&thread2, NULL, function2, NULL);
	pthread_create(&thread1, NULL, function1, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);
	pthread_join(thread4, NULL);
	return 0;
}

void *function1()
{
	while (1) {
		pthread_mutex_lock(&mutex);
		if (count == 1) {
			printf("\t\tFunction 1\n");
			count++;
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return NULL;
		} else {
			pthread_cond_wait(&cond, &mutex);
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}

void *function2()
{
	while (1) {
		pthread_mutex_lock(&mutex);
		if (count == 2) {
			printf("\t\tFunction 2\n");
			count++;
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return NULL;
		} else {
			pthread_cond_wait(&cond, &mutex);
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}

void *function3()
{
	while (1) {
		pthread_mutex_lock(&mutex);
		if (count == 3) {
			printf("\t\tFunction 3\n");
			count++;
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return NULL;
		} else {
			pthread_cond_wait(&cond, &mutex);
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}

void *function4()
{
	while (1) {
		pthread_mutex_lock(&mutex);
		if (count == 4) {
			printf("\t\tFunction 4\n");
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&cond);
			return NULL;
		} else {
			pthread_cond_wait(&cond, &mutex);
			pthread_mutex_unlock(&mutex);
		}
	}
	return NULL;
}
