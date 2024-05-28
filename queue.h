#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "process.h"

typedef struct Node {
    Process* process;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

Queue* createQueue();
void enqueue(Queue* queue, Process* process); 
void enqueuePriority(Queue* queue, Process* process);         
void enqueueSjf(Queue* queue, Process* process);      
Process* dequeue(Queue* queue);
bool isEmpty(Queue* queue);
Process* peek(Queue* queue);
void reorderProcess(Queue* queue, Process* process, int schedulingType);
void reorderReadyQueue(Queue* queue);  
void freeQueue(Queue* queue);
void executeWaitingQueue(Queue* waitingQueue, Queue* readyQueue);
void printQueue(Queue* queue);
Process* findProcessByArrivalTime(Queue* queue, int timeUnit);
Process* dequeueByPid(Queue* queue, int pid);
Process* highestResponseRatio(Queue* queue, int currentTime);

#endif // 
