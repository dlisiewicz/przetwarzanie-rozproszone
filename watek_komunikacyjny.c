#include "watek_komunikacyjny.h"
#include "main.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void* startKomWatek(void* ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (stan != InFinish) {
        debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        sem_wait(&local_clock_semaphore);
        if (pakiet.timestamp > local_clock){
            local_clock = pakiet.timestamp;
        }
        sem_post(&local_clock_semaphore);

        switch (status.MPI_TAG) {
            case REQUEST:
                debug("Dosałem REQ od %d", status.MPI_SOURCE)
                handleRequest(pakiet, queueHead);
                
                sem_wait(&local_clock_semaphore);
                local_clock++;
                sem_post(&local_clock_semaphore);

                sendPacket(0, status.MPI_SOURCE, ACK);
                break;

            case ACK:
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
                ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie?
                                Zastanówcie się. */
                if(ackCount == size - 1 && isElementInNElements(queueHead, rank, MIEJSCA, type, target)) {
                    pthread_cond_signal(&condition);
                }         
                break;
            case RELEASE:
                debug("Dostałem RELEASE od %d", status.MPI_SOURCE);
                removeNode(&queueHead, status.MPI_SOURCE);
                if(ackCount == size - 1 && isElementInNElements(queueHead, rank, MIEJSCA, type, target)) {
                    pthread_cond_signal(&condition);
                }
                break;
            default:
                break;
        }
    }
}
