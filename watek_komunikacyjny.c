#include "watek_komunikacyjny.h"
#include "main.h"

void* startKomWatek(void* ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
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
                sem_wait(&local_clock_semaphore);
                debug("Dosałem REQ od %d", status.MPI_SOURCE)
                handleRequest(pakiet);
                local_clock++;
                sendPacket(0, status.MPI_SOURCE, ACK);
                sem_post(&local_clock_semaphore);
                break;
            case GUIDE_REQUEST:
                sem_wait(&local_clock_semaphore);
                debug("Dosałem GUIDE_REQUEST od %d", status.MPI_SOURCE)
                handleGuideRequest(pakiet);
                local_clock++;
                sendPacket(0, status.MPI_SOURCE, GUIDE_ACK);
                sem_post(&local_clock_semaphore);
                break;
            case ACK:
                sem_wait(&local_clock_semaphore);
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
                ackCount++;
                if(ackCount == size - 1 && isElementInNElements(queueHead, rank, MIEJSCA, type, target)) {
                    pthread_cond_signal(&condition);
                }
                sem_post(&local_clock_semaphore);  
                break;
            case GUIDE_ACK:
                sem_wait(&local_clock_semaphore);
                debug("Dostałem GUIDE_ACK od %d, mam już %d", status.MPI_SOURCE, guideAckCount);
                guideAckCount++;
                if (!(guideAckCount == size - 1 && isElementInNElements(guideQueueHead, rank, 1, type, target))) {
                    pthread_cond_signal(&condition);
                }
                sem_post(&local_clock_semaphore); 
                break;
            case RELEASE:
                sem_wait(&local_clock_semaphore);
                debug("Dostałem RELEASE od %d", status.MPI_SOURCE);
                removeNode(&queueHead, status.MPI_SOURCE);
                removeNode(&guideQueueHead, status.MPI_SOURCE);

                if(ackCount == size - 1 && isElementInNElements(queueHead, rank, MIEJSCA, type, target)) {
                    pthread_cond_signal(&condition);
                }
                if (guideAckCount == size - 1 && isElementInNElements(guideQueueHead, rank, 1, type, target)) {
                    pthread_cond_signal(&condition);
                }
                sem_post(&local_clock_semaphore); 
                break;
            default:
                break;
        }
    }
}
