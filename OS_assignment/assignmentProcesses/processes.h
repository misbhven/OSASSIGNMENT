#ifndef PROCESSES_H
#define PROCESSES_H

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>

typedef struct _request_item {
    int src;
    int dest;
} request_item;

typedef struct _buffer_item {
    int front;
    int back;
    int bufSize;
    request_item buffer[100];
} buffer_item;

typedef struct _lift_item {
    int liftName;
    int moveTotal;
    int requestTotal;
    int prev;
} lift_item;

typedef struct _vars_item {
    int requestsInBuffer;
    int requestsConsumed;
    int requestsProduced;
    int totalRequests;
    int totalMoves;
    int flag;
    int sleepTime;
    pthread_mutex_t mutex;
    pthread_cond_t canProduce;
    pthread_cond_t canConsume;
} vars_item;

#include "fileIO.h"

void processInit(int m, int t);
void lift_R();
void lift(int id);

#endif
