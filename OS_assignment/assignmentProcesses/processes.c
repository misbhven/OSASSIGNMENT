/*
File: processes.c
Author: Dylan Travers
ID: 19128043
Program: lift_sim

Purpose: init all shared memory blocks, fork all processes, clean up.
*******************************************************************************/


#include "processes.h"

/* is passed the cmd line args, init all shared memory for processes */
void processInit(int m, int t)
{
    int varsFd, bufFd, i;
	vars_item* vars = NULL;
    buffer_item* buf = NULL;
    const char *varsName = "OS_VARS";
    const char *bufName = "OS_BUF";
    pthread_condattr_t condattr;
    pthread_mutexattr_t mtxattr;



	/* initialize the buffer control variables */
    varsFd = shm_open(varsName, O_CREAT | O_RDWR, 0666);
    if (varsFd < 0)
    {
        perror("vars shmopen error\n");
    }
    ftruncate(varsFd, sizeof(vars_item));
    pthread_mutexattr_init(&mtxattr);
    pthread_mutexattr_setpshared(&mtxattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&condattr);
    pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);

	vars = mmap(0, sizeof(vars_item), PROT_READ | PROT_WRITE, MAP_SHARED, varsFd, 0);
    vars->requestsInBuffer = 0;
    vars->requestsConsumed = 0;
    vars->requestsProduced = 0;
    vars->totalMoves = 0;
    vars->totalRequests = 0;
    vars->flag = 0;
    vars->sleepTime = t;
    pthread_mutex_init(&vars->mutex, &mtxattr);
    pthread_cond_init(&vars->canProduce, &condattr);
    pthread_cond_init(&vars->canConsume, &condattr);

	/* initialize the buffer */
	bufFd = shm_open(bufName, O_CREAT | O_RDWR, 0666);
    if (bufFd < 0)
    {
        perror("buf shmopen error\n");
    }
    ftruncate(bufFd, sizeof(buffer_item));
    buf = mmap(0, sizeof(buffer_item), PROT_READ | PROT_WRITE, MAP_SHARED, bufFd, 0);
    buf->front = 0;
    buf->back = 0;
    buf->bufSize = m;

	for (i = 0; i < 4; i++)
	{
		int	pid = fork();

		if (pid == 0 && i == 0)
			lift_R();
		else if (pid == 0)
			lift(i);
		/*else pid == 0, which implies the parent process --
		it loops around to start the next child.*/
	}
	for (i = 0; i < 4; i++)
	{
		wait(NULL);
	}

    /* log totals in out file */
    totalLogger(vars->requestsConsumed, vars->totalMoves);


    /* clean up all mutexes, condition variables, mutex attriburtes and shared mem */
    pthread_mutex_destroy(&vars->mutex);
    pthread_cond_destroy(&vars->canProduce);
    pthread_cond_destroy(&vars->canConsume);

    pthread_mutexattr_destroy(&mtxattr);
    pthread_condattr_destroy(&condattr);

    munmap(vars, sizeof(vars_item));
    munmap(buf, sizeof(buffer_item));

    close(varsFd);
    close(bufFd);

    shm_unlink(varsName);
    shm_unlink(bufName);
}

/*
 LIFT REQUEST PRODUCER
 */
void lift_R()
{
	int	i, varsFd, bufFd, allReqBufFd;
	vars_item* vars;
	buffer_item* buffer;
    buffer_item* allReqBuf;
    request_item request;

    /* all requests buffer */
    allReqBufFd = shm_open("OS_REQUESTS", O_RDWR, 0666);
    allReqBuf = mmap(NULL, sizeof(buffer_item), PROT_READ | PROT_WRITE, MAP_SHARED, allReqBufFd, 0);

    /* buffer variables */
    varsFd = shm_open("OS_VARS", O_RDWR, 0666);
    vars = mmap(NULL, sizeof(vars_item), PROT_READ | PROT_WRITE, MAP_SHARED, varsFd, 0);

    /* the bounded buffer itself */
    bufFd = shm_open("OS_BUF", O_RDWR, 0666);
    buffer = mmap(NULL, sizeof(buffer_item), PROT_READ | PROT_WRITE, MAP_SHARED, bufFd, 0);

    vars->totalRequests = allReqBuf->bufSize;
	printf("Producer starting\n");

    /*begin*/
	for (i = 0; i < allReqBuf->bufSize; i++)
	{
        /* get next request */
        request = allReqBuf->buffer[allReqBuf->front];
        allReqBuf->front = (allReqBuf->front + 1) % allReqBuf->bufSize;

		/* critical section */
		pthread_mutex_lock(&vars->mutex);
        while(vars->requestsInBuffer == buffer->bufSize)
        {
            pthread_cond_wait(&vars->canProduce, &vars->mutex);
        }

        /* lock attained */
        buffer->buffer[buffer->back] = request;
    	buffer->back = (buffer->back + 1) % buffer->bufSize;
        vars->requestsInBuffer++;
        vars->requestsProduced++;

        producerLogger(request.src, request.dest, vars->requestsProduced);

		/* end critical section */
        pthread_cond_signal(&vars->canConsume);
        pthread_mutex_unlock(&vars->mutex);
	}
	printf("producer exiting\n");

    /*clean up*/
    munmap(vars, sizeof(vars_item));
    munmap(buffer, sizeof(buffer_item));
    munmap(allReqBuf, sizeof(buffer_item));

    close(varsFd);
    close(bufFd);
    close(allReqBufFd);
	exit(0);
}
/*
 LIFT CONSUMERS
 */
void lift(int id)
{
    int varsFd, bufFd, movement;
	vars_item* vars;
	buffer_item* buffer;
    request_item request;
    lift_item* lift;

    /* init a lift struct */
    lift = (lift_item*)malloc(sizeof(lift_item));
    lift->liftName = id;
    lift->prev = 1;
    lift->moveTotal = 0;
    lift->requestTotal = 0;

    /* buffer varaibles */
    varsFd = shm_open("OS_VARS", O_RDWR, 0666);
    vars = mmap(0, sizeof(vars_item), PROT_READ | PROT_WRITE, MAP_SHARED, varsFd, 0);

    /* the bounded buffer */
    bufFd = shm_open("OS_BUF", O_RDWR, 0666);
    buffer = mmap(0, sizeof(buffer_item), PROT_READ | PROT_WRITE, MAP_SHARED, bufFd, 0);

	printf("\nLift %d starting\n", id);

    /*begin*/
	while(vars->flag == 0)
	{
		/* critical section*/
		pthread_mutex_lock(&vars->mutex);
        while((vars->requestsInBuffer == 0) && (vars->flag ==0))
        {
            pthread_cond_wait(&vars->canConsume, &vars->mutex);
        }

        /* lock attained, check flag to see if there are any requests left */
        if(vars->flag == 0)
        {
            request = buffer->buffer[buffer->front];
    		buffer->front = (buffer->front + 1) % buffer->bufSize;
            vars->requestsInBuffer--;
            vars->requestsConsumed++;
            lift->requestTotal++;
            movement = abs((lift->prev - request.src) + (request.src - request.dest));
            lift->moveTotal += movement;
            vars->totalMoves += movement;

            consumerLogger(request.src, request.dest, movement, lift, vars);

            lift->prev = request.dest;

        }
        /* if that was the last request, flip flag to let other processes know */
		if (vars->requestsConsumed == vars->totalRequests)
        {
            vars->flag = 1;
        }
		/* end critical section */
        pthread_cond_signal(&vars->canProduce);
        pthread_mutex_unlock(&vars->mutex);
        sleep(vars->sleepTime);
	}

	printf("\nLift %d exiting\n", id);
    /* clean up */
    munmap(vars, sizeof(vars_item));
    munmap(buffer, sizeof(buffer_item));

    close(varsFd);
    close(bufFd);
    free(lift);
	exit(0);
}
