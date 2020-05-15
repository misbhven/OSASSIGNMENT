#AUTHOR Dylan Travers
#ID: 19128043
#PROGRAM: lift_sim

#Flags
CC = gcc
CFLAGS = --std=c99 -Wall -Wextra -Wformat=2 -Wswitch-default -Wswitch-enum -Wpointer-arith -Wbad-function-cast -Wstrict-overflow=5 -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code -Wfloat-equal -Wstrict-aliasing -Wredundant-decls -g -O0 -fno-omit-frame-pointer -fno-common -Wdouble-promotion -Wcast-align -Winit-self -D_XOPEN_SOURCE=500
TARGET = lift_sim_B
OBJECTS = main.o processes.o fileIO.o

#files
#PLEASE INSERT FLAG -lrt FOR CURTIN LAB COMPUTERS AFTER -lpthread FLAG
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) -lpthread -lrt

main.o : main.c main.h processes.h fileIO.h
	$(CC) $(CFLAGS) -c main.c

processes.o : processes.c processes.h fileIO.h
	$(CC) $(CFLAGS) -c processes.c

fileIO.o: fileIO.c fileIO.h processes.h
	$(CC) $(CFLAGS) -c fileIO.c

#CLEAN
clean :
	rm -rf lift_sim_B
	rm -rf *.o
	rm -rf sim_out.txt

#RUN
run :
	./lift_sim_B 10 1

valgrind :
	valgrind ./lift_sim_B 10 1

helgrind :
	valgrind --tool=helgrind ./lift_sim_B 10 1
