/*
File: main.c
Author: Dylan Travers
ID: 19128043
Program: lift_sim

Purpose: runs the main method, error checks cmd line args
*******************************************************************************/
#include "main.h"

int main(int argc, char **argv)
{
    printf("\n\n\n----------\n* lift_sim_B (processes)\n* Starting...\n----------\n\n\n");
    /* cmd line arguements for buffer size and sleep time */
    int m, t, allReqBufFd;
    buffer_item* allReqBuf;
    const char* reqBufName = "OS_REQUESTS";

    /* check that number of arguements == 3, otherwise provide usage info */
    if(argc == 3)
    {
        m = atoi(argv[1]);
        t = atoi(argv[2]);
    }
    /* usage information for running ./program */
    else
    {
        printf("Usage: %s <buffer size> <time for elevator to sleep", argv[0]);
        exit(-1);
    }

    allReqBufFd = shm_open(reqBufName, O_CREAT | O_RDWR, 0666);
    if(allReqBufFd < 0)
    {
        perror("allReqBuf shmopen error\n");
    }
    ftruncate(allReqBufFd, sizeof(buffer_item));
    allReqBuf = mmap(NULL, sizeof(buffer_item), PROT_READ | PROT_WRITE, MAP_SHARED, allReqBufFd, 0);

    allReqBuf = inFileInit(allReqBuf);


    printf("----------\n* Running lifts...\n----------\n\n\n");
    processInit(m, t);
    printf("----------\n*lift_sim_B (processes)\n* Done!\n* Check sim_out.txt for results.\n----------\n\n\n");

    munmap(allReqBuf, sizeof(buffer_item));
    close(allReqBufFd);
    shm_unlink(reqBufName);
}
