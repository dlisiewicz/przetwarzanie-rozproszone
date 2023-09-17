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

/* boolean */
#define TRUE 1
#define FALSE 0
#define SEC_IN_STATE 1
#define STATE_CHANGE_PROB 10

#define ROOT 0

#define HOTELE 2
#define MIEJSCA 2
#define NIEBIESCY 4
#define FIOLETOWI 4


#define NIEBIESKI 0
#define FIOLETOWY 1

/* tutaj TYLKO zapowiedzi - definicje w main.c */
extern int rank;
extern int size;
extern int ackCount;
extern int local_clock;
extern int type;
extern int target;
extern char type_array[2][10];
extern struct list_element *queueHead;
extern pthread_mutex_t mutex;
extern pthread_cond_t condition;
extern sem_t local_clock_semaphore;
extern pthread_t threadKom;


/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta

   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl
   bold/normal;kolor [RANK] FORMAT:argumenty doklejone z wywołania debug poprzez
   __VA_ARGS__
                       "%c[%d;%dm"       wyczyszczenie
   atrybutów    27,0,37 UWAGA: 27 == kod ascii escape. Pierwsze %c[%d;%dm ( np
   27[1;10m ) definiuje styl i kolor literek Drugie   %c[%d;%dm czyli 27[0;37m
   przywraca domyślne kolory i brak pogrubienia (bolda) ...  w definicji makra
   oznacza, że ma zmienną liczbę parametrów

*/
#ifdef DEBUG
#define debug(FORMAT, ...)                                                                                                               \
printf("%c[%d;%dm [%d][%s][%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, type_array[type], local_clock, ##__VA_ARGS__, 27, \
         0, 37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT, ...)                                                                                                             \
printf("%c[%d;%dm [%d][%s][%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, type_array[type], local_clock, ##__VA_ARGS__, 27, \
         0, 37);
#endif
