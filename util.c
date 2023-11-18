#include "util.h"
#include "main.h"

MPI_Datatype MPI_PAKIET_T;

state_t stan = InRun;

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
              {"zwolnienie sekcji krytycznej", RELEASE},
              {"prośbę o sekcję krytyczną", GUIDE_REQUEST},
              {"potwierdzenie", GUIDE_ACK}};

const char const* tag2string(int tag)
{
    for (int i = 0; i < sizeof(tagNames) / sizeof(struct tagNames_t); i++)
    {
        if (tagNames[i].tag == tag) return tagNames[i].name;
    }
    return "<unknown>";
}

void inicjuj_typ_pakietu()
{
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

void sendPacket(packet_t* pkt, int destination, int tag)
{
    int freepkt = 0;
    if (pkt == 0) {
        pkt = malloc(sizeof(packet_t));
        freepkt = 1;
    }

    pkt->source_rank = rank;
    pkt->timestamp = local_clock;
    pkt->type = type;
    pkt->target = target;

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
void handleGuideRequest(packet_t pakiet){
    insertNode(&guideQueueHead, pakiet.timestamp, pakiet.source_rank, pakiet.type, pakiet.target);
    sortList(&guideQueueHead);
    printList(guideQueueHead);
    return;
}

void sortList(struct list_element** head) {
    struct list_element* current;
    struct list_element* next;
    int swapped;

    if (*head == NULL) {
        return;
    }

    do {
        swapped = 0;
        current = *head;

        while (current->next != NULL) {
            next = current->next;

            if (current->timestamp > next->timestamp) {
                if (current == *head) {
                    *head = next;
                } else {
                    struct list_element* prev = *head;
                    while (prev->next != current) {
                        prev = prev->next;
                    }
                    prev->next = next;
                }
                current->next = next->next;
                next->next = current;
                swapped = 1;
            } else if (current->timestamp == next->timestamp && current->source_rank > next->source_rank) {
                if (current == *head) {
                    *head = next;
                } else {
                    struct list_element* prev = *head;
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

void insertNode(struct list_element** head, int timestamp, int source_rank, int type, int target) {
    struct list_element* new_node = malloc(sizeof(struct list_element));
    new_node->source_rank = source_rank;
    new_node->timestamp = timestamp;
    new_node->type = type;
    new_node->target = target;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        struct list_element* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void removeNode(struct list_element** head, int source_rank) {
    if (*head == NULL) {
        return;
    }

    struct list_element* current = *head;
    struct list_element* prev = NULL;

    if (current != NULL && current->source_rank == source_rank) {
        *head = current->next;
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

void printList(struct list_element* head) {
    debug("---------------")
    debug("RANK: %d", rank);

    struct list_element* current = head;

    while (current != NULL) {
        debug("Source: %d, ts: %d, type: %s, target: %d", current->source_rank, current->timestamp, type_array[current->type], current->target);
        current = current->next;
    }
    debug("---------------")
}

int isElementInNElements(struct list_element* head, int source_rank, int n, int type, int target) {
    struct list_element* current = head;
    int count = 0;

    while (current != NULL && count < n) {
        if(current != NULL && current->target == target && current->type != type){
            return 0;
        }

        if (current->source_rank == source_rank) {
            return 1;
        }

        current = current->next;
        if(current != NULL && current->target == target) {
            count++;
        }

    }

    return 0;
}