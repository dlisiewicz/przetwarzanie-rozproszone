#include "watek_glowny.h"

#include "main.h"

void mainLoop()
{
    srandom(rank);
    int tag;
    int perc;

    while (stan != InFinish)
    {
        switch (stan)
        {
            case InRun:
                perc = random() % 100;
                if (perc < 25)
                {
                    //debug("Perc: %d", perc);
                    println("Ubiegam się o sekcję krytyczną") 
                    //debug("Zmieniam stan na wysyłanie");
                    packet_t* pkt = malloc(sizeof(packet_t));

                    ackCount = 0;

                    sem_wait(&local_clock_semaphore);
                    local_clock++;
                    for (int i = 0; i <= size - 1; i++) {
                        if (i != rank) {
                            sendPacket(pkt, i, REQUEST);
                        }
                    }
                    sem_post(&local_clock_semaphore);


                    insertNode(&queueHead, pkt->timestamp, pkt->source_rank, pkt->type, pkt->target); //type i target do zmiany
                    sortList(&queueHead);
                    printList(queueHead);

                    changeState(InWant);
                    free(pkt);
                }
                debug("Skończyłem myśleć");
                break;

            case InWant:
                println("Czekam na wejście do sekcji krytycznej")
                    // tutaj zapewne jakiś muteks albo zmienna warunkowa
                    // bo aktywne czekanie jest BUE
                pthread_mutex_lock(&mutex);
                while (!(ackCount == size - 1 && isElementAmongFirst(queueHead, rank, MIEJSCA))) {
                    pthread_cond_wait(&condition, &mutex);
                } 
                pthread_mutex_unlock(&mutex);
                changeState(InSection);
                break;

            case InSection:
                // tutaj zapewne jakiś muteks albo zmienna warunkowa
                println("Jestem w sekcji krytycznej") 
                sleep(5);
                // if ( perc < 25 ) {
                debug("Perc: %d", perc);
                println("Wychodzę z sekcji krytycznej") debug("Zmieniam stan na wysyłanie");
                packet_t* pkt = malloc(sizeof(packet_t));

                for (int i = 0; i <= size - 1; i++) {
                    if (i != rank) {
                        sendPacket(pkt, i, RELEASE);
                    }
                }
                ackCount = 0;
                removeNode(&queueHead, rank);
                changeState(InRun);
                free(pkt);
                //}
                break;

            default:
                break;
        } 
        sleep(SEC_IN_STATE);
    }
}
