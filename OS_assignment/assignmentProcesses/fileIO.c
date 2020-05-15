/*
File: fileIO.c
Author: Dylan Travers
ID: 19128043
Program: lift_sim

Purpose: to handle all file io between other functions and sim_in and sim_out
         files and their respective values
*******************************************************************************/

#include "fileIO.h"

buffer_item *inFileInit(buffer_item *buf)
{
    /* file pointer for sim_in.txt */
    FILE* inFile;
    /* each individual line that is read in */
    char line[MAX_LINE_LENGTH];
    char* inFileName = "sim_input.txt";
    int lines, i, src, dest;
    request_item request;


    if((inFile = fopen(inFileName, "r")) != NULL)
    {
        /* get lines in sim_in.txt */
        lines = getLines(inFile);
        rewind(inFile);
        /* iterate through lines */
        if(lines >= 50 && lines <= 100)
        {
            buf->bufSize = lines;
            buf->front = buf->back = 0;

            for(i = 0; i < lines; i++)
            {
                if(fgets(line, MAX_LINE_LENGTH, inFile) != NULL)
                {
                    /* get instructions from line */
                    sscanf(line, "%d %d", &src, &dest);
                    /* make sure instructions are within the floors 1-20 */
                    if(src >= 1 && src <= 20)
                    {
                        if(dest >= 1 && dest <= 20)
                        {
                            /* add request to buffer */
                            request.src = src;
                            request.dest = dest;
                            buf->buffer[buf->back] = request;
                        	buf->back = (buf->back + 1) % buf->bufSize;
                        }
                        else
                        {
                            printf("ERROR: Found incorrect input in %s, %d is an invalid floor. Line: %d\n", inFileName, dest, i+1);
                        }
                    }
                    else
                    {
                        printf("ERROR: Found incorrect input in %s, %d is an invalid floor. Line: %d\n", inFileName, src, i+1);
                    }
                }
                else
                {
                    perror("ERROR: sim_input.txt incorrectly formatted\n");
                }
            }
            fclose(inFile);
        }
        else
        {
            perror("ERROR: sim_input.txt is not working\n");
        }
    }
    else
    {
        perror("ERROR: Lift requests in sim_input.txt is out side of range 50 <= number of requests <= 100\n");
    }
    return buf;
}

int getLines(FILE* file)
{
    char ch;
    int lines;
    lines = 0;

    for(ch = fgetc(file); !feof(file); ch = fgetc(file))
    {
        if(ch == '\n')
        {
            lines++;
        }
    }
    return lines;
}

void consumerLogger(int src, int dest, int movement, lift_item* lift, vars_item* vars)
{
    FILE* logfile;

    if((logfile = fopen("sim_out.txt", "a")) != NULL)
    {
        fprintf(logfile, "-------------------------------------------------------\n");
        fprintf(logfile, "Lift %d consumed: floor %d to floor %d\n", lift->liftName, src, dest);
        fprintf(logfile, "Previous Floor: %d\n", lift->prev);
        fprintf(logfile, "Detailed operation:\n\tGo from floor %d to floor %d\n\t", lift->prev, src);
        fprintf(logfile, "Go from floor %d to floor %d\n\t", src, dest);
        fprintf(logfile, "#movement for this request: %d\n\t", movement);
        fprintf(logfile, "#Total movement: %d\n\t", vars->totalMoves);
        fprintf(logfile, "#Total requests: %d\n", vars->requestsConsumed);
        fprintf(logfile, "Current Floor: %d\n", dest);
    }
    else
    {
        perror("ERROR: couldnt open output file");
    }
    fclose(logfile);
}

void totalLogger(int requests, int moves)
{
    FILE* logfile;

    if((logfile = fopen("sim_out.txt", "a")) != NULL)
    {
        fprintf(logfile, "-------------------------------------------------------\n");
        fprintf(logfile, "Total Number of Requests: %d\nTotal movements: %d\n", requests, moves);
        fprintf(logfile, "-------------------------------------------------------\n");
    }
    else
    {
        perror("ERROR: couldnt open output file");
    }
    fclose(logfile);
}

void producerLogger(int src, int dest, int totalRequests)
{
    FILE* logfile;

    if((logfile = fopen("sim_out.txt", "a")) != NULL)
    {
        fprintf(logfile, "-------------------------------------------------------\n");
        fprintf(logfile, "Produced: floor %d to floor %d.\nRequest Num: %d\n", src, dest, totalRequests);
    }
    else
    {
        perror("ERROR: couldnt open output file");
    }
    fclose(logfile);
}
