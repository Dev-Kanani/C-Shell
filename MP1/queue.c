#include "all_directories.h"
#include "queue.h"

Link append(Link l, char *val)
{
    if (l == NULL)
    {
        l = malloc(sizeof(Node));
        l->next = NULL;
        strcpy(l->val, val);
        return l;
    }

    Link temp = (Link)malloc(sizeof(Node));
    temp->next = NULL;
    strcpy(temp->val, val);
    l->next = temp;
    return temp;
}

void create(Queue *Q)
{
    *Q = (Queue)malloc(sizeof(stQueue));
    (*Q)->head = NULL;
    (*Q)->tail = NULL;
    (*Q)->size = 0;
    return;
}

void enQueue(Queue Q, char *val)
{
    Q->tail = append(Q->tail, val);
    if (Q->head == NULL)
    {
        Q->head = Q->tail;
    }
    Q->size++; // Update size after enqueue
}

char *deQueue(Queue Q)
{
    if (Q->head == NULL)
    {
        printf("QUEUE IS EMPTY\n");
        return NULL;
    }

    char *ans = (char *)malloc(sizeof(char) * 1000);
    strcpy(ans, Q->head->val);

    Link temp = Q->head;
    Q->head = Q->head->next;
    free(temp);

    if (Q->head == NULL)
    {
        Q->tail = NULL;
    }

    Q->size--; // Update size after dequeue
    return ans;
}

void prit(Queue Q)
{
    if (Q->head == NULL)
    {
        return;
    }

    Link temp = Q->head;

    while (temp != NULL)
    {
        printf("%s", temp->val);
        temp = temp->next;
    }
}

void destroyQueue(Queue Q)
{
    while (Q->head != NULL)
    {
        Link e = Q->head->next;
        free(Q->head);
        Q->head = e;
    }
    free(Q);
}