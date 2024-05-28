#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, Process* process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

void enqueuePriority(Queue* queue, Process* process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->front == NULL || queue->front->process->priority > process->priority) {
        newNode->next = queue->front;
        queue->front = newNode;
        if (queue->rear == NULL) {
            queue->rear = newNode;
        }
        return;
    }

    Node* current = queue->front;
    while (current->next != NULL && current->next->process->priority <= process->priority) {
        current = current->next;
    }
    
    newNode->next = current->next;
    current->next = newNode;

    if (newNode->next == NULL) {
        queue->rear = newNode;
    }
}

void enqueueSjf(Queue* queue, Process* process) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->front == NULL || queue->front->process->cpuBurstTime > process->cpuBurstTime) {
        newNode->next = queue->front;
        queue->front = newNode;
        if (queue->rear == NULL) {
            queue->rear = newNode;
        }
        return;
    }

    Node* current = queue->front;
    while (current->next != NULL && current->next->process->cpuBurstTime <= process->cpuBurstTime) {
        current = current->next;
    }
    
    newNode->next = current->next;
    current->next = newNode;

    if (newNode->next == NULL) {
        queue->rear = newNode;
    }
}

Process* dequeue(Queue* queue) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }
    Node* temp = queue->front;
    Process* process = temp->process;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    return process;
}

bool isEmpty(Queue* queue) {
    return queue->front == NULL;
}

Process* peek(Queue* queue) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }
    return queue->front->process;
}

void reorderProcess(Queue* queue, Process* process, int schedulingType) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return;
    }

    Node* current = queue->front;
    Node* previous = NULL;
    while (current != NULL && current->process != process) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Process not found in the queue\n");
        return;
    }

    if (previous == NULL) { 
        queue->front = current->next;
    } else {
        previous->next = current->next;
    }

    if (current->next == NULL) { 
        queue->rear = previous;
    }

    free(current);

    if (schedulingType == 1) {
        enqueueSjf(queue, process);
    } else if (schedulingType == 2){
        enqueuePriority(queue, process);
    }
}



void freeQueue(Queue* queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}


void executeWaitingQueue(Queue* waitingQueue, Queue* readyQueue) {
    Node* prev = NULL;
    Node* current = waitingQueue->front;
    while (current != NULL) {
        current->process->ioBurstTime--;
        
        if (current->process->ioBurstTime == 0) {
            if (prev == NULL) { 
                waitingQueue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (current->next == NULL) { 
                waitingQueue->rear = prev;
            }
            Process* readyProcess = current->process;
            Node* temp = current;
            current = current->next;
            free(temp);
            enqueuePriority(readyQueue, readyProcess);
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void reorderReadyQueue(Queue* queue) {
    if (queue->front == NULL || queue->front->next == NULL) {
        return;
    }

    Node* current = queue->front;
    while (current != NULL) {
        Node* nextNode = current->next;
        while (nextNode != NULL) {
            if (current->process->arrivalTime == nextNode->process->arrivalTime && current->process->priority > nextNode->process->priority) {
                Process* temp = current->process;
                current->process = nextNode->process;
                nextNode->process = temp;
            }
            nextNode = nextNode->next;
        }
        current = current->next;
    }
}

void printQueue(Queue* queue) {
    Node* current = queue->front;
    while (current != NULL) {
        Process* process = current->process;
        printf("PID: %d, Arrival Time: %d, Priority: %d, CPU Burst Time: %d, IO Burst Time: %d\n",
               process->pid, process->arrivalTime, process->priority, process->cpuBurstTime, process->ioBurstTime);
        current = current->next;
    }
}

Process* findProcessByArrivalTime(Queue* queue, int timeUnit) {
    Node* current = queue->front;
    while (current != NULL) {
        if (current->process->arrivalTime == timeUnit) {
            return current->process;
        }
        current = current->next;
    }
    return NULL;
}

