#ifndef UTILH
#define UTILH
#include "main.h"

typedef struct {
    int timestamp;
    int source_rank;
    int type;
    int target;
} packet_t;

struct list_element{
    int timestamp;
    int source_rank;
    int type;
    int target;
    struct list_element *next;
};

#define NITEMS 4

#define ACK 1
#define REQUEST 2
#define RELEASE 3
#define APP_PKT 4
#define FINISH 5
#define GUIDE_REQUEST 6
#define GUIDE_ACK 7

extern MPI_Datatype MPI_PAKIET_T;
void inicjuj_typ_pakietu();

void sendPacket(packet_t* pkt, int destination, int tag);

typedef enum {
    InRun,
    InMonitor,
    InWant,
    InSection,
    InFinish,
    InWantGuide,
    InSectionGuide,
    InSectionCleaner
} state_t;

extern state_t stan;
extern pthread_mutex_t stateMut;
void changeState(state_t);
void handleRequest(packet_t);
void handleGuideRequest(packet_t);
void sortList(struct list_element** head);
void insertNode(struct list_element** head, int timestamp, int source_rank, int type, int target);
void removeNode(struct list_element** head, int source_rank);
void printList(struct list_element* head);
int isElementInNElements(struct list_element* head, int source_rank, int x, int type, int target);
#endif
