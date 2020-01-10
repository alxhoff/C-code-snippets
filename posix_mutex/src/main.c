#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

pthread_t threads[2];
pthread_mutex_t lock;
int counter = 0;

void *increaseCounter(void *args){
    pthread_mutex_lock(&lock);

    counter += 1;

    printf("Counter: %d\n", counter);

    pthread_mutex_unlock(&lock);
}

int main(int argc, char* argv[])
{
    int err; 

    if(pthread_mutex_init(&lock, NULL)){
        perror("Mutex failed to init");
        return -1;
    }

    for(int i =0; i < 2; i++){
        err = pthread_create(&(threads[i]), NULL, &increaseCounter, NULL);

        assert(!err);
    }

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_mutex_destroy(&lock);

    return 0;
}
