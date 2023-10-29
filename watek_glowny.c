#include "watek_glowny.h"
#include "main.h"

void mainLoop()
{

    srandom(rank);
    int perc;
    int hotel_id;

    while (stan != InFinish)
    {
        switch (stan)
        {
            case InRun:
                packet_t* pkt = malloc(sizeof(packet_t));
                perc = random() % 100;
                if (perc < 25)
                {
                    //debug("Perc: %d", perc);

                    target = random() % HOTELE;
                    hotel_id = target;
                    println("Ubiegam się o pokój w hotelu o id: %d", target) 
                    ackCount = 0;
                    guideAckCount = 0;
                    sem_wait(&local_clock_semaphore);
                    local_clock++;
                    for (int i = 0; i <= size - 1; i++) {
                        if (i != rank) {
                            sendPacket(pkt, i, REQUEST);
                        }
                    }
                    sem_post(&local_clock_semaphore);

                    insertNode(&queueHead, pkt->timestamp, pkt->source_rank, pkt->type, pkt->target);
                    sortList(&queueHead);
                    //printList(queueHead);

                    changeState(InWant);
                }
                debug("Skończyłem myśleć");
                break;
            case InWant:
                println("Czekam na wejście do pokoju w hotelu o id: %d", target)
                pthread_mutex_lock(&mutex);
                while (!(ackCount == size - 1 && isElementInNElements(queueHead, rank, MIEJSCA, type, target))) {
                    pthread_cond_wait(&condition, &mutex);
                } 
                pthread_mutex_unlock(&mutex);

                if (type == SPRZATACZ) {
                    changeState(InSectionCleaner);
                }
                else {
                    changeState(InSection);
                }
                
                break;
            case InSectionCleaner:
                debug("-----------------------------")
                println("Jestem w hotelu o id: %d", target) 
                debug("-----------------------------")

                sleep(random() % 5);
                println("Wychodzę z hotelu o id: %d", hotel_id)
                for (int i = 0; i <= size - 1; i++) {
                    if (i != rank) {
                        sendPacket(pkt, i, RELEASE);
                    }
                }
                removeNode(&queueHead, rank);
                changeState(InRun);
                free(pkt);
                break;
            case InSection:
                // tutaj zapewne jakiś muteks albo zmienna warunkowa
                debug("-----------------------------")
                println("Jestem w hotelu o id: %d", target) 
                debug("-----------------------------")

                target = random() % PRZEWODNICY;
                println("Ubiegam się o przewodnika o id: %d", target)
                
                sem_wait(&local_clock_semaphore);
                local_clock++;
                for (int i = 0; i <= size - 1; i++) {
                    if (i != rank) {
                        sendPacket(pkt, i, GUIDE_REQUEST);
                    }
                }
                sem_post(&local_clock_semaphore);

                insertNode(&guideQueueHead, pkt->timestamp, pkt->source_rank, pkt->type, pkt->target);
                sortList(&guideQueueHead);

                changeState(InWantGuide);
                break;
            case InWantGuide:
                println("Czekam na mojego przewodnika o id: %d", target)
                pthread_mutex_lock(&mutex);
                while (!(guideAckCount == size - 1 && isElementInNElements(guideQueueHead, rank, 1, type, target))) {
                    pthread_cond_wait(&condition, &mutex);
                } 
                pthread_mutex_unlock(&mutex);
                changeState(InSectionGuide);
                break;
            case InSectionGuide:
                println("Idę z przewodnikiem o id: %d na wyprawę", target)
                sleep(random() % 5);
                println("Wróciłem z wyprawy, wychodzę z hotelu o id: %d", hotel_id)
                for (int i = 0; i <= size - 1; i++) {
                    if (i != rank) {
                        sendPacket(pkt, i, RELEASE);
                    }
                }
                removeNode(&queueHead, rank);
                removeNode(&guideQueueHead, rank);
                changeState(InRun);
                free(pkt);
                break;
            default:
                break;
        } 
        sleep(SEC_IN_STATE);
    }
}
