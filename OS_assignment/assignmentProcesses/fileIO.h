#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "processes.h"

#define MAX_LINE_LENGTH 8

buffer_item *inFileInit(buffer_item *buf);
int getLines(FILE* file);
void consumerLogger(int src, int dest, int movement, lift_item* lift, vars_item* vars);
void totalLogger(int requests, int moves);
void producerLogger(int src, int dest, int totalRequests);

#endif
