#include "scheduling.h"
#include "utils.h"
#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "evaluate.h"

/**
 * @brief FCFS
 * 
 * @param processes 
 */
void fcfsScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    while(n > 0){
        // assumes that processes are sorted by arrival time
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
            }
        }

        // no available process to run, consider idle
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            // although the running queue is empty, the processes in the waiting queue are processed by -1 second
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }

        // available process to run, run the process at the front of the queue
        Process* runProcess = peek(readyQueue);

        // ioTime refers to countdown of I/O operation within burst
        if(runProcess->ioTime == 0){
            // ioTime -1
            runProcess->ioTime--;
            // running queue -> waiting queue
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            // run the next process in the same time unit
            // if there is no next process, continue
            if (!isEmpty(readyQueue)) {
                runProcess = peek(readyQueue);
            } else {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 0);
                runProcess = NULL;
                timeUnit++;
                continue;
            }
        }

        // decrement all processes in the waiting queue by 1 second
        // move the processes whose I/O is completed waiting queue -> ready queue
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        // record the execution for Gantt chart display
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // process termination, dequeue the process from the running queue
        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit+1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "FCFS");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}


void sjfScheduling(Process* processes) {

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    // Enqueue processes in the ready queue sorted by shortest burst time (tiebreaker: arrival time)
    // Execute in arrival order in the while loop
    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueueSjf(readyQueue, &processesCopy[i]);
    }

    Process* runProcess = NULL;
    int deactivatePreemptionCounter = 0;
    
    while(n > 0 ){
        Node* currentNode;

        // Non-preemptive SJF, deactivate preemption until cpuBurstTime is 0
        if(!deactivatePreemptionCounter){
            // Loop through ready queue
            for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                // Since the ready queue is already sorted by burst time in the setup phase, execute in arrival order
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        // No executable process, consider idle
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 1);
            timeUnit++;
            continue;
        }

        if(timeUnit < 50){
            // printf("Timeunit: %d, pid: %d\n", timeUnit, runProcess->pid);
        }

        // Process is running
        if(deactivatePreemptionCounter == 0){
            deactivatePreemptionCounter = runProcess->cpuBurstTime - 1;
        } else {
            deactivatePreemptionCounter--;
        }

        // Handle I/O
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);

            // Move from ready queue to waiting queue. Execute next process if available.
            // Ex. RQ = [P3, P4] => Available(RQ) = nil
            // Check for the next executable process in the for loop
            int isAvailableNext = 0;
            if(!isEmpty(readyQueue)){
                for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                    if (currentNode -> process -> arrivalTime <= timeUnit){
                        runProcess = currentNode -> process;
                        isAvailableNext = 1;
                        break;
                    }
                }
            }
            if (!isAvailableNext) {
                // The just moved process is the last process Ex. RQ = [P3] => RQ = []
                // No next executable process Ex. RQ = [P3, P4] => Available(RQ) = nil
                // If runProcess (current executable process) is NULL, consider idle
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 1);
                // Indicate that there is no current process
                runProcess = NULL;
                // Allow preemption, otherwise infinite loop
                deactivatePreemptionCounter = 0;
                timeUnit++;
                continue;
            } else {
                // Execute next process
                deactivatePreemptionCounter = runProcess->cpuBurstTime - 1;
            }
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 1);

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Process termination
        if (runProcess->cpuBurstTime == 0) {
            // Process runs until this time unit and terminates in the next time unit
            runProcess->completionTime = timeUnit + 1;
            // Ready queue is sorted by burst time and executed in arrival order, so the current process may not be at the front
            // Thus, dequeue by pid
            dequeueByPid(readyQueue, runProcess->pid);
            // Prevent the dequeued process from being executed in the next round
            runProcess = NULL;
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "SJF");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

// Smaller priority value means higher priority
void preemptiveSjfScheduling(Process* processes) {

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();
    
    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    while(n > 0){
        // enqueueSjf sorts by shortest cpuBurstTime (tiebreaker: arrival time)
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueueSjf(readyQueue, &processesCopy[i]);
            }
        }

        // No executable process, consider idle
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 1);
            timeUnit++;
            continue;
        }

        // Executable process exists, execute the process at the front of the queue
        // Non-preemptive SJF: the ready queue is filled initially
        // Preemptive SJF: only executable processes are enqueued, sorted by cpuBurstTime
        // In preemptive SJF, the front of the queue is the next process to execute
        Process* runProcess = peek(readyQueue);

        // Handle I/O
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            if (!isEmpty(readyQueue)) {
                runProcess = peek(readyQueue);
            } else {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 1);
                runProcess = NULL;
                timeUnit++;
                continue;
            }
        }

        // Indicate preemption
        // runProcess: currently running process
        // currentProcess: previously running process
        if(currentProcess != NULL && runProcess->cpuBurstTime < currentProcess->cpuBurstTime){
            /** Ex
             * CPU Burst Time: A:4, B:2, C:3
             * Process A: runs T3 ~ T5 (remaining: 2)
             * Process B: arrives at T4 (remaining: 2)
             * Process C: runs T5 ~ T8 (remaining: 0)
             * At T8, the next process should be A or B. Since B arrived while A was running, B should run (longer waiting time)
             * If A was preempted, it should be reordered behind B
             */
            reorderProcess(readyQueue, currentProcess, 1);
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 1);

        // Assign the current process to the previous process
        currentProcess = runProcess;

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Process termination
        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit + 1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "P-SJF");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void priorityScheduling(Process* processes) {

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    // Enqueue processes in the ready queue sorted by priority (tiebreaker: arrival time)
    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueuePriority(readyQueue, &processesCopy[i]);
    }

    Process* runProcess = NULL;
    int deactivatePreemptionCounter = 0;
    
     while(n > 0){
        Node* currentNode;

        if(!deactivatePreemptionCounter){
            for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                // Check if process is executable
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        // No executable process, consider idle
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 2);
            timeUnit++;
            continue;
        }

        // Process is running, do not interrupt
        if(deactivatePreemptionCounter == 0){
            deactivatePreemptionCounter = runProcess->cpuBurstTime - 1;
        } else {
            deactivatePreemptionCounter--;
        }

        // Handle I/O
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            int isAvailableNext = 0;
            if(!isEmpty(readyQueue)){
                for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                    if (currentNode -> process -> arrivalTime <= timeUnit){
                        runProcess = currentNode -> process;
                        isAvailableNext = 1;
                        break;
                    }
                }
            }
            if (!isAvailableNext) {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 2);
                runProcess = NULL;
                deactivatePreemptionCounter = 0;
                timeUnit++;
                continue;
            } else {
                deactivatePreemptionCounter = runProcess->cpuBurstTime - 1;
            }
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 2);

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Process termination
        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit + 1;
            dequeueByPid(readyQueue, runProcess->pid);
            runProcess = NULL;
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "Priority");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void preemptivePriorityScheduling(Process* processes) {

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    while(n > 0){
        // Enqueue processes in the ready queue sorted by priority (tiebreaker: arrival time)
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueuePriority(readyQueue, &processesCopy[i]);
            }
        }

        // No executable process, consider idle
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 2);
            timeUnit++;
            continue;
        }

        // Executable process exists, execute the process at the front of the queue
        Process* runProcess = peek(readyQueue);

        // Handle I/O
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            if (!isEmpty(readyQueue)) {
                runProcess = peek(readyQueue);
            } else {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 1);
                timeUnit++;
                continue;
            }
        }

        // Reorder due to preemption
        if(currentProcess != NULL && runProcess->priority < currentProcess->priority){
            reorderProcess(readyQueue, currentProcess, 2);
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 2);

        currentProcess = runProcess;

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Process termination
        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit + 1;
            dequeue(readyQueue);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "PP");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void roundRobinScheduling(Process* processes, int quantum){

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* currentProcess = NULL;
    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    int quantumCounter = 0;
    // Manage remaining burst time separately as it may be less than the time quantum
    int runTime = 0;

    while(n > 0){
        // Enqueue: Enqueue processes in the ready queue in arrival order
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
                continue;
            }
        }

        // Executable process exists, execute the process at the front of the queue
        currentProcess = peek(readyQueue);

        // No executable process, consider idle
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }
        
        // quantumCounter: process runs without interruption for the allocated time, non-preemptive
        if (!quantumCounter) {
            dequeue(readyQueue);
            // Process termination
            if(currentProcess->cpuBurstTime == 0){
                currentProcess->completionTime = timeUnit;
                n--;
                currentProcess = NULL;
                // As dequeue() is done here, scheduling termination should also be handled here
                // n: number of alive processes
                if(n == 0){
                    break;
                }
            } else { // Re-enqueue in the ready queue
                enqueue(readyQueue, currentProcess);
            }

            // Execute next process
            if (!isEmpty(readyQueue)) {
                currentProcess = peek(readyQueue);
            } else {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 0);
                currentProcess = NULL;
                timeUnit++;
                continue;
            }
            
            // Handle case where remaining burst time is less than the time quantum
            runTime = (currentProcess->cpuBurstTime < quantum) ? currentProcess->cpuBurstTime : quantum;
            quantumCounter = --runTime;
        } else {
            quantumCounter--;
        }

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, currentProcess->pid, timeUnit, timeUnit + 1);
        currentProcess->cpuBurstTime--;
        currentProcess->ioTime--;

        // Handle I/O
        if (currentProcess->ioTime == 0) {
            dequeue(readyQueue);
            enqueue(waitingQueue, currentProcess);
            quantumCounter = 0;
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "RR");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}

void hrrnScheduling(Process* processes) {

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess *ganttQueue = NULL;
    int queueCount = 0;

    Process* runProcess = NULL;
    int deactivatePreemptionCounter = 0;

    for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
        enqueue(readyQueue, &processesCopy[i]);
    }

    while(n > 0){
        // HRRN is non-preemptive
        if(!deactivatePreemptionCounter){
            // highestResponseRatio(): Returns HRR among executable processes
            runProcess = highestResponseRatio(readyQueue, timeUnit);
            if(runProcess != NULL){
                deactivatePreemptionCounter = runProcess->cpuBurstTime - 1;
            }
        } else {
            deactivatePreemptionCounter--;
        }

        // No executable process, consider idle
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }

        // Handle I/O
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            
            if (!isEmpty(readyQueue)) {
                runProcess = peek(readyQueue);
            } else {
                enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
                executeWaitingQueue(waitingQueue, readyQueue, 1);
                runProcess = NULL;
                timeUnit++;
                continue;
            }
        }

        // Handle waiting queue
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        // Handle Gantt chart
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Process termination
        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit + 1;
            dequeueByPid(readyQueue, runProcess->pid);
            n--;
        }

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT, "HRRN");
    printGanttChart(ganttQueue, queueCount);
    freeQueue(readyQueue);
    freeQueue(waitingQueue);
    free(ganttQueue);
}
