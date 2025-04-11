#ifndef QUEUE_H
#define QUEUE_H

#include "all_directories.h"

typedef struct Node
{
    char val[1000];
    struct Node *next;
} Node;

typedef Node *Link;

typedef struct stQueue
{
    Link head, tail;
    int size;
} stQueue;

typedef stQueue *Queue;

void create(Queue *Q);
void enQueue(Queue Q, char *val);
char *deQueue(Queue Q);
void destroyQueue(Queue Q);
Link append(Link l, char *val);
void prit(Queue Q);

#endif
