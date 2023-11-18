#ifndef MAINH
#define MAINH

#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

#define ROOT 0

#define HOTELE 2
#define MIEJSCA 2
#define NIEBIESCY 3
#define FIOLETOWI 3
#define PRZEWODNICY 2
#define SPRZATACZE 2

#define NIEBIESKI 0
#define FIOLETOWY 1
#define SPRZATACZ 2

extern int rank;
extern int size;
extern int ackCount;
extern int guideAckCount;
extern int local_clock;
extern int type;
extern int target;
extern char type_array[3][10];
extern struct list_element *queueHead;
extern struct list_element *guideQueueHead;
extern pthread_mutex_t mutex;
extern pthread_cond_t condition;
extern sem_t local_clock_semaphore;
extern pthread_t threadKom;

#ifdef DEBUG
#define debug(FORMAT, ...)                                                                                                               \
printf("%c[%d;%dm [%d][%s][%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, type_array[type], local_clock, ##__VA_ARGS__, 27, \
         0, 37);
#else
#define debug(...) ;
#endif

#define println(FORMAT, ...)                                                                                                             \
printf("%c[%d;%dm [%d][%s][%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, type_array[type], local_clock, ##__VA_ARGS__, 27, \
         0, 37);
#endif
