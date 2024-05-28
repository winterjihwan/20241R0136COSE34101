#include "scheduling.h"
#include "utils.h"
#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "evaluate.h"

/**
 * @brief FCFS 스케줄링
 * @details 도착 순으로 정렬되어 있는 프로세스 시퀀스를 인자로 받았다는  것을 전제한다 (main()에서 sortProcessesByArrivalTime() 호출)
 * @param processes 프로세스 배열
 */
void fcfsScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
            }
        }

        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        Process* runProcess = peek(readyQueue);

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void sjfScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueueSjf(readyQueue, &processesCopy[i]);
    }

    Process* runProcess = NULL;
    int deactivatePreeemptionCounter = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================
    
    while(n > 0 ){
        Node* currentNode;

        if(!deactivatePreeemptionCounter){
            for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        if(deactivatePreeemptionCounter == 0){
            deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
        }else{
            deactivatePreeemptionCounter--;
        }

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeueByPid(readyQueue, runProcess->pid);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void preemptiveSjfScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();
    
    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueueSjf(readyQueue, &processesCopy[i]);
            }
        }

        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        Process* runProcess = peek(readyQueue);

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        if(currentProcess != NULL && runProcess->cpuBurstTime < currentProcess->cpuBurstTime){
            reorderProcess(readyQueue, currentProcess, 1);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        currentProcess = runProcess;

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void priorityScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueuePriority(readyQueue, &processesCopy[i]);
    }

    Process* runProcess = NULL;
    int deactivatePreeemptionCounter = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    
     while(n > 0){
        Node* currentNode;

        if(!deactivatePreeemptionCounter){
            for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        if(deactivatePreeemptionCounter == 0){
            deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
        }else{
            deactivatePreeemptionCounter--;
        }

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeueByPid(readyQueue, runProcess->pid);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void preemptivePriorityScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueuePriority(readyQueue, &processesCopy[i]);
            }
        }

        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        Process* runProcess = peek(readyQueue);

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        if(currentProcess != NULL && runProcess->priority < currentProcess->priority){
            reorderProcess(readyQueue, currentProcess, 2);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        currentProcess = runProcess;

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void roundRobinScheduling(Process* processes, int quantum){
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    int quantumCounter = 0;
    int runTime = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
                continue;
            }
        }

        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }
        
        currentProcess = peek(readyQueue);
        
        if (!quantumCounter) {
            dequeue(readyQueue);
            if(currentProcess->cpuBurstTime == 0){
                currentProcess->completionTime = timeUnit;
                n--;
                if(n==0){
                    break;
                }
            } else {
                enqueue(readyQueue, currentProcess);
            }

            currentProcess = peek(readyQueue);
            runTime = (currentProcess->cpuBurstTime < quantum) ? currentProcess->cpuBurstTime : quantum;
            quantumCounter = --runTime;
        } else {
            quantumCounter--;
        }

        enqueueGanttProcess(&ganttQueue, &queueCount, currentProcess->pid, timeUnit, timeUnit + 1);
        currentProcess->cpuBurstTime--;
        currentProcess->ioTime--;

        if (currentProcess->ioTime == 0) {
            enqueue(waitingQueue, currentProcess);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        timeUnit++;
    }

    // evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void hrrnScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    Process* runProcess = NULL;
    int deactivatePreeemptionCounter = 0;

    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueue(readyQueue, &processesCopy[i]);
    }

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        if(!deactivatePreeemptionCounter){
            runProcess = highestResponseRatio(readyQueue, timeUnit);
            if(runProcess != NULL){
                deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
            }
        } else {
            deactivatePreeemptionCounter--;
        }

        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue);
            timeUnit++;
            continue;
        }

        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        executeWaitingQueue(waitingQueue, readyQueue);

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeueByPid(readyQueue, runProcess->pid);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}