#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void thr_err(int ret, char *str)
{
    if (ret != 0) {
        fprintf(stderr, "%s:%s\n", str, strerror(ret));
        pthread_exit(NULL);
    }
}

struct dumpling {
    int value;
    struct dumpling *next;
};

struct dumpling *head = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ready = PTHREAD_COND_INITIALIZER;

void *producer(void *arg)
{
    for (int i=0; i < 10; i++) {
        struct dumpling *tmp = malloc(sizeof(struct dumpling));
        tmp->value = rand() % 1000 + 1;

        pthread_mutex_lock(&mutex);
        tmp->next = head;
        head = tmp;
        pthread_mutex_unlock(&mutex);

        printf("+++Producer %lu give %d\n", pthread_self(), tmp->value);
        pthread_cond_signal(&ready);
        sleep(rand() % 2);
    }
    return NULL;
}

void *consumer(void *arg)
{
    struct dumpling *tmp;
    for (int i=0; i < 10; i++) {
        pthread_mutex_lock(&mutex);
        while (head == NULL) {
            pthread_cond_wait(&ready, &mutex);
        }
        tmp = head;
        head = head->next;
        pthread_mutex_unlock(&mutex);

        printf("-----Cumsumer %lu have %d\n", pthread_self(), tmp->value);
        free(tmp);
        sleep(rand()%3);
    }
    return NULL;
}

int main(int argc, char *argv)
{
    int ret;
    srand((unsigned)time(NULL));

    pthread_t pid, cid;
    /* int pthread_create(pthread_t *restrict thread,
                          const pthread_attr_t *restrict attr,
                          typeof(void *(void *)) *start_routine,
                          void *restrict arg);
    */
    pthread_create(&pid, NULL, producer, NULL);
    pthread_create(&cid, NULL, consumer, NULL);

    pthread_join(pid, NULL);
    pthread_join(cid, NULL);
    return 0;
}
