# assignmentProcesses
Student: Dylan Travers
ID: 19128043
Assignment: OS 2020 Sem 1, part B
PROGRAM: lift_sim_B (processes)

An Operating Systems COMP200 assignment to design a Bounded-Buffer/Producer-Consumer
solution for multiple simulated elevators servicing different floors of a building. Written in C using pthreads.

This directory contains source code, Makefile and sample input of 51 lift requests. 

ATTENTION: may require the inclusion of the linker flag -lrt on some systems, may cause errors on others. 
Add/remove it if necessary from the Makefile

to compile:
    ~/assignmentProcesses$ make

to run with sample input buffer size = 5 and sleep time = 5:
    ~/assignmentProcesses$ make run

to run with valgrind (im not sure how well this works with multiprocessing) and sample cmd args:
    ~/assignmentProcesses$ make valgrind

to run with vaglrind tool helgrind (im not sure how well it handles pthread condition variables and multiple processes)
    ~/assignmentProcesses make helgrind

