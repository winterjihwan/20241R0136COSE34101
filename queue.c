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

    // 큐가 비어있다면
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

    // 큐가 비어있거나 front 프로세스 preemption
    if (queue->front == NULL || queue->front->process->priority > process->priority) {
        newNode->next = queue->front;
        queue->front = newNode;
        if (queue->rear == NULL) {
            queue->rear = newNode;
        }
        return;
    }

    Node* current = queue->front;
    // while 루프가 끝나면 current -> next는 프로세스 우선순위가 더 낮은 프로세스를 가리킴
    // 즉, 동일한 프라이오리티를 가진 프소세스 중 맨 뒤에 위치하게 된다
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
    // 큐가 비어있다
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
    // 디큐하지 않음
    return queue->front->process;
}

// 동일한 우선순위 시 (priority, cpuBurstTime) 프로세스를 뒤로 재배치
void reorderProcess(Queue* queue, Process* process, int schedulingType) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return;
    }

    Node* current = queue->front;
    Node* previous = NULL;
    // 프로세스 위치 탐색
    while (current != NULL && current->process != process) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Process not found in the queue\n");
        return;
    }
    
    // 지우고자 하는 프로세스가 맨 앞
    if (previous == NULL) { 
        queue->front = current->next;
    } else {
        previous->next = current->next;
    }

    if (current->next == NULL) { 
        queue->rear = previous;
    }

    free(current);

    // 각자 방식으로 재배치
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

// 웨이팅 큐에 있는 프로세스 ioBurstTime을 모두 -1씩 감소
void executeWaitingQueue(Queue* waitingQueue, Queue* readyQueue, int schedulingType) {
    Node* prev = NULL;
    Node* current = waitingQueue->front;
    while (current != NULL) {
        current->process->ioBurstTime--;
        
        // ioBurstTime이 0이 되면 readyQueue로 이동
        if (current->process->ioBurstTime == 0) {
            // 맨앞
            if (prev == NULL) { 
                waitingQueue->front = current->next;
            } else { // 중간
                prev->next = current->next;
            }
            // 맨뒤
            if (current->next == NULL) { 
                waitingQueue->rear = prev;
            }
            Process* readyProcess = current->process;
            Node* temp = current;
            current = current->next;
            free(temp);
            if(schedulingType == 0){
                enqueue(readyQueue, readyProcess);
            } else if(schedulingType == 1){
                enqueueSjf(readyQueue, readyProcess);
            } else if(schedulingType == 2){
                enqueuePriority(readyQueue, readyProcess);
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
}


// 디버깅 목적
void printQueue(Queue* queue) {
    Node* current = queue->front;
    while (current != NULL) {
        Process* process = current->process;
        printf("PID: %d, Arrival Time: %d, Priority: %d, CPU Burst Time: %d, IO Burst Time: %d\n",
               process->pid, process->arrivalTime, process->priority, process->cpuBurstTime, process->ioBurstTime);
        current = current->next;
    }
}


Process* dequeueByPid(Queue* queue, int pid) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return NULL;
    }

    Node* current = queue->front;
    Node* previous = NULL;

    while (current != NULL && current->process->pid != pid) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Process with PID %d not found in the queue\n", pid);
        return NULL;
    }

    // 지우고자 하는 프로세스가 맨 앞
    if (previous == NULL) { 
        queue->front = current->next;
    } else {
        previous->next = current->next;
    }

    if (current->next == NULL) { 
        queue->rear = previous;
    }

    Process* process = current->process;
    free(current);
    return process;
}


Process* highestResponseRatio(Queue* queue, int currentTime) {
    if (queue->front == NULL) {
        return NULL;
    }

    Node* current = queue->front;
    Process* highestRRProcess = NULL;
    double highestResponseRatio = -1.0;

    while (current != NULL) {
        if (current->process->arrivalTime <= currentTime) {
            int waitingTime = currentTime - current->process->arrivalTime;
            // rr = (waitingTime + cpuBurstTime) / cpuBurstTime
            double responseRatio = ((double)(waitingTime + current->process->cpuBurstTime)) / current->process->cpuBurstTime;

            // 더 높은 RR을 가진 프로세스를 찾음
            if (highestRRProcess == NULL || responseRatio > highestResponseRatio) {
                highestResponseRatio = responseRatio;
                highestRRProcess = current->process;
            }
        }
        current = current->next;
    }

    return highestRRProcess;
}