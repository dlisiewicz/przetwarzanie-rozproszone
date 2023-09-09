#include "util.h"
#include "main.h"

MPI_Datatype MPI_PAKIET_T;

/*
 * w util.h extern state_t stan (czyli zapowiedź, że gdzieś tam jest definicja
 * tutaj w util.c state_t stan (czyli faktyczna definicja)
 */
state_t stan = InRun;

/* zamek wokół zmiennej współdzielonej między wątkami.
 * Zwróćcie uwagę, że każdy proces ma osobą pamięć, ale w ramach jednego
 * procesu wątki współdzielą zmienne - więc dostęp do nich powinien
 * być obwarowany muteksami
 */
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;

struct tagNames_t
{
  const char* name;
  int tag;
}

tagNames[] = {{"pakiet aplikacyjny", APP_PKT},
              {"finish", FINISH},
              {"potwierdzenie", ACK},
              {"prośbę o sekcję krytyczną", REQUEST},
              {"zwolnienie sekcji krytycznej", RELEASE}};

const char const* tag2string(int tag)
{
    for (int i = 0; i < sizeof(tagNames) / sizeof(struct tagNames_t); i++)
    {
        if (tagNames[i].tag == tag) return tagNames[i].name;
    }
    return "<unknown>";
}
/* tworzy typ MPI_PAKIET_T
 */
void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
        brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t),
        MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int blocklengths[NITEMS] = {1, 1, 1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[NITEMS];

    offsets[0] = offsetof(packet_t, timestamp);
    offsets[1] = offsetof(packet_t, source_rank);
    offsets[2] = offsetof(packet_t, type);
    offsets[3] = offsetof(packet_t, target);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);
    MPI_Type_commit(&MPI_PAKIET_T);
}

/* opis patrz util.h */
void sendPacket(packet_t* pkt, int destination, int tag)
{
    int freepkt = 0;
    if (pkt == 0) {
        pkt = malloc(sizeof(packet_t));
        freepkt = 1;
    }
    
    MPI_Send(pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    debug("Wysyłam %s do %d", tag2string(tag), destination);
    if (freepkt) {
        free(pkt);
    } 
}

void changeState(state_t newState)
{
    pthread_mutex_lock(&stateMut);
    if (stan == InFinish) {
        pthread_mutex_unlock(&stateMut);
        return;
    }
    stan = newState;
    pthread_mutex_unlock(&stateMut);
}
void handleRequest(packet_t pakiet){
    insertNode(&queueHead, pakiet.timestamp, pakiet.source_rank, pakiet.type, pakiet.target);
    sortList(&queueHead);
    printList(queueHead);
    return;
}

void sortList(struct list_element** queueHead) {
    struct list_element* current;
    struct list_element* next;
    int swapped;

    if (*queueHead == NULL) {
        return;
    }

    do {
        swapped = 0;
        current = *queueHead;

        while (current->next != NULL) {
            next = current->next;

            if (current->timestamp > next->timestamp) {
                // Zamiana węzłów
                if (current == *queueHead) {
                    *queueHead = next;
                } else {
                    struct list_element* prev = *queueHead;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = next;
                }
                current->next = next->next;
                next->next = current;
                swapped = 1;
            } else if (current->timestamp == next->timestamp && current->source_rank > next->source_rank) {
                if (current == *queueHead) {
                    *queueHead = next;
                } else {
                    struct list_element* prev = *queueHead;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = next;
                }
                current->next = next->next;
                next->next = current;
                swapped = 1;
            }

            current = next;
        }
    } while (swapped);
}

void insertNode(struct list_element** queueHead, int timestamp, int source_rank, int type, int target) {
    struct list_element* new_node = malloc(sizeof(struct list_element));
    new_node->source_rank = source_rank;
    new_node->timestamp = timestamp;
    new_node->type = type;
    new_node->target = target;
    new_node->next = NULL;

    if (*queueHead == NULL) {
        *queueHead = new_node;
    } else {
        struct list_element* current = *queueHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void removeNode(struct list_element** queueHead, int source_rank) {
    if (*queueHead == NULL) {
        return;
    }

    struct list_element* current = *queueHead;
    struct list_element* prev = NULL;

    if (current != NULL && current->source_rank == source_rank) {
        *queueHead = current->next;
        free(current);
        return;
    }

    while (current != NULL && current->source_rank != source_rank) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    prev->next = current->next;
    free(current);
}

void printList(struct list_element* queueHead) {
    debug("RANK: %d\n", rank);
    struct list_element* current = queueHead;

    while (current != NULL) {
        debug("ID: %d, TS: %d\n", current->source_rank, current->timestamp);
        current = current->next;
    }
}