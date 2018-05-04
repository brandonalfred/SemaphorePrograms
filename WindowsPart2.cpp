#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

const int SIZE = 3;
queue<int> seatQ;
queue<int> waitingQ;

//int isHelping = 1;
//int seatsFull = 0;
//int seatsEmpty = SIZE;

sem_t isHelping;
sem_t seatsFull;
sem_t seatsEmpty;
bool helping;
bool sleepMsgPrinted;
bool firstTime = true;

void *producerRunner(void *param);
void *consumerRunner(void *param);
void *taRunner(void *param);
void *studentRunner(void *param);
void helpStudent();
void sitInHall(int);

int main(int argc, char *argv[]) {
    int timerLimit;
    int numOfStudentThreads;
    srand(time(NULL));

    sem_init(&isHelping, 0, 1);
    sem_init(&seatsFull, 0, 0);
    sem_init(&seatsEmpty, 0, SIZE);

    if (argc != 3) {
        fprintf(stderr,"usage: a.out <sleep time> <number of student threads>\n");
        return -1;
    }

    timerLimit = atoi(argv[1]);
    numOfStudentThreads = atoi(argv[2]);

    pthread_t taThread;
    pthread_t *studentThreads = new pthread_t[numOfStudentThreads];
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    time_t timerEnd = time(NULL) + timerLimit;

    pthread_create(&taThread, &attr, taRunner, NULL);

    for (int i = 0; i < numOfStudentThreads; i++) {
        pthread_create(&studentThreads[i], &attr, studentRunner, &i);
    }

    while(time(NULL) <= timerEnd);

    printf("\n");

    sem_destroy(&isHelping);
    sem_destroy(&seatsFull);
    sem_destroy(&seatsEmpty);

    return 0;
}

void *studentRunner(void *param) {
    do {
        int index = *(int *)param;
        int decision = rand() % 2;

        sem_wait(&seatsEmpty);
        sem_wait(&isHelping);

        if(decision == 1) {
            sitInHall(index);
        }

        sem_post(&isHelping);
        sem_post(&seatsFull);
    } while(true);
}

void *taRunner(void *param) {
    do {
        sem_wait(&seatsFull);
        sem_wait(&isHelping);

        if(seatQ.empty() && waitingQ.empty()) {
            helping = false;
            if(!sleepMsgPrinted)
                printf("\nNo more students need help... TA going back to sleep... ZZZzzz");
            sleepMsgPrinted = true;
        }
        else {
            helping = true;
            if(sleepMsgPrinted || firstTime)
                printf("\n\nTA is woken up... grumble...");

            helpStudent();
            sleep(2);
        }

        firstTime = false;

        sem_post(&isHelping);
        sem_post(&seatsEmpty);
    } while(true);
}

void helpStudent() {
    if(!seatQ.empty()) {
        int num = seatQ.front();
        seatQ.pop();
        sleepMsgPrinted = false;
        printf("\n\tTA is helping student %d", num);
    }
    else {
        int num = waitingQ.front();
        waitingQ.pop();
        sleepMsgPrinted = false;
        printf("\n\tTA is helping student %d", num);
    }
}

void sitInHall(int id) {
    if(waitingQ.size() == 3) {
        printf("\n\t\t\tNo more chairs?!?! :'( Student %d returns to programming", id);
        sleep(1);
    }
    else {
        if(helping) {
            waitingQ.push(id);
            printf("\n\t\tStudent %d takes a seat in the hall (%d student(s) waiting)", id, (int)waitingQ.size());
        }
        else
            seatQ.push(id);
    }
}
