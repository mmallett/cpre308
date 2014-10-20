/*
    Matt Mallett
    CprE 308 Project 2
    life.h
    header file for Conway's Threaded Game of Life
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define DEFAULT_SIZE    4
#define DEFAULT_THREADS 1
#define DEFAULT_GENS    2

int
*gen_x, *gen_y;

int
grid_size, num_threads, num_gens;

char
*input, *output;

typedef struct
{
    int start;
    int end;
}arg_t;

pthread_mutex_t mut;
pthread_cond_t  cv;
int             threads_running;

void
init_gen_x();

void
dump_gen_x();

int
evolve(int,int);

int get_index(int,int);
