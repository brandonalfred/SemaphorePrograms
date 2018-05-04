#include <iostream>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

using namespace std;

const int SIZE = 10;
queue<int> q;

sem_t mu;
sem_t full;
sem_t Empty;

void *producerRunner(void *param);
void *consumerRunner(void *param);
void insert_item(int);
void remove_item(int);

int main(int argc, char *argv[]) {
    int timerLimit;
    int numOfProducers;
    int numOfConsumers;

    srand(time(NULL));

    sem_init(&mu, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&Empty, 0, SIZE);


    if (argc != 4)
    {
        fprintf(stderr,"usage: a.out <sleep time> <number of producers> <number of consumers>\n");
        return -1;
    }

    timerLimit = atoi(argv[1]);
    numOfProducers = atoi(argv[2]);
    numOfConsumers = atoi(argv[3]);

    pthread_t *producers = new pthread_t[numOfProducers];
    pthread_t *consumers = new pthread_t[numOfConsumers];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    time_t timerEnd = time(NULL) + timerLimit;

    for (int i = 0; i < numOfProducers; i++) {
        pthread_create(&producers[i], &attr, producerRunner, &i);
    }

    for (int i = 0; i < numOfConsumers; i++) {
        pthread_create(&consumers[i], &attr, consumerRunner, &i);
    }

    while(time(NULL) <= timerEnd);
    
    sem_destroy(&mu);
    sem_destroy(&full);
    sem_destroy(&Empty);
    
    printf("\n");

    return 0;
}

void *producerRunner(void *param) {
    int index = *(int *)param;

    do {
        sem_wait(&Empty);
        sem_wait(&mu);

        insert_item(index);

        sem_post(&mu);
        sem_post(&full);
    } while(true);
}

void *consumerRunner(void *param) {
    int index = *(int *)param;

    do {
        sem_wait(&full);
        sem_wait(&mu);
        
        remove_item(index);

        sem_post(&mu);
        sem_post(&Empty);
    } while(true);
}

void insert_item(int index) {
    int randomNum = rand() % 1000;

    q.push(randomNum);
    printf("\nProducer %d produced: %d", index, randomNum);
}

void remove_item(int index) {
    int num = q.front();
    q.pop();
    printf("\n\tConsumer %d consumed: %d", index, num);
}
