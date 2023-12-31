#include "main.h"
#include "watek_glowny.h"
#include "watek_komunikacyjny.h"

int rank, size;
int ackCount = 0;
int guideAckCount = 0;
int local_clock = 0;
int type = 0;
int target = 0;
char type_array[3][10] = {"Niebieski", "Fioletowy", "Sprzątacz"};
struct list_element *queueHead = NULL;
struct list_element *guideQueueHead = NULL;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
sem_t local_clock_semaphore;

pthread_t threadKom;

void finalizuj()
{
    pthread_mutex_destroy(&stateMut);
    println("czekam na wątek \"komunikacyjny\"\n");
    pthread_join(threadKom, NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
    sem_destroy(&local_clock_semaphore);
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE:
            printf("Brak wsparcia dla wątków, kończę\n");
            fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
            MPI_Finalize();
            exit(-1);
            break;
        case MPI_THREAD_FUNNELED:
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać "
                    "wołania do biblioteki mpi\n");
            break;
        case MPI_THREAD_SERIALIZED:
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
            break;
        case MPI_THREAD_MULTIPLE:
            printf("Pełne wsparcie dla wątków\n");
            break;
        default:
            printf("Nikt nic nie wie\n");
    }
}

int main(int argc, char** argv)
{
    MPI_Status status;
    int provided;
    sem_init(&local_clock_semaphore, 0, 1);
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    srand(rank);
    inicjuj_typ_pakietu();

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != NIEBIESCY + FIOLETOWI + SPRZATACZE) {
        println("Liczba kosmitów niebieskich + fioletowych + sprzątaczy musi być równa liczbie procesów\n");
        println("Niebiescy: %d\n Fioletowi: %d\n Sprzątacze: %d\n Procesy: %d\n", NIEBIESCY, FIOLETOWI, SPRZATACZE, size);
        finalizuj();
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank<NIEBIESCY) {
        type = NIEBIESKI;
    }
    else if(rank >= NIEBIESCY && rank < NIEBIESCY + FIOLETOWI){
        type = FIOLETOWY;
    }
    else {
        type = SPRZATACZ;
    }

    pthread_create(&threadKom, NULL, startKomWatek, 0);

    mainLoop();

    finalizuj();
    return 0;
}
