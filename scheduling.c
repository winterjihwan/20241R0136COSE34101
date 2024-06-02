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
        // main.c에서 이미 arrivalTime으로 정렬되어 있다
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            // running queue는 empty여도 waiting queue에 있는 프로세스들은 -1초 처리
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }

        // 실행 가능한 프로세스 O, 큐 맨 앞 프로세스 실행
        Process* runProcess = peek(readyQueue);

        // ioTime은 'Process XX는 3초 실행 후 I/O 2초간 실행' 에서의 2초를 의미한다
        if(runProcess->ioTime == 0){
            // ioTime -1 처리
            runProcess->ioTime--;
            // running queue -> waiting queue
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            // 같은 타임 유닛 내에서 다음 프로세스 실행
            runProcess = peek(readyQueue);
        }

        // waiting queue 전부 -1초 처리, I/O completion시 waiting queue -> ready queue
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        // 간트차트 출력을 위해 실행 기록 기입
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료: running queue -> terminated
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

    // Shortest burst time 순으로 레디큐 기입 (타이브레이커: 도착순)
    // while문에서 도착 순으로 실행
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

        // non preemptive SJF이기에 cpuBurstTime이 0이 되기 전까지 preemption deactivate
        if(!deactivatePreeemptionCounter){
            // 레디큐 루프
            for(currentNode = readyQueue -> front; currentNode != NULL; currentNode = currentNode -> next){
                // Setup단계에서 이미 burst time 순으로 정렬되어 있기에 도착순으로 실행하면 된다
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 1);
            timeUnit++;
            continue;
        }

        // 프로세스 실행 중
        if(deactivatePreeemptionCounter == 0){
            deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
        }else{
            deactivatePreeemptionCounter--;
        }

        // I/O 처리
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 1);

        // 간트차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료
        if (runProcess->cpuBurstTime == 0) {
            // 프로세스는 이번 타임 유닛까지 실행 후 다음 타임 유닛에 종료
            runProcess->completionTime = timeUnit+1;
            // 레디큐는 burst time으로 정렬되어있고 도착 순으로 실행하기에 현재 실행 중인 프로세스가 큐의 맨 앞이 아닐 수 있다
            // 따라서 pid로 dequeue
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

// 작은 프라이오리티 값이 높은 우선순위를 가진다
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
        // enqueueSjf는 cpuBurstTime이 작은 순으로 정렬한다 (타이브레이커: 도착순)
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueueSjf(readyQueue, &processesCopy[i]);
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 1);
            timeUnit++;
            continue;
        }

        // 실행 가능한 프로세스 O, 큐 맨 앞 프로세스 실행
        // non preemptive sjf: 레디큐가 처음부터 차있다
        // preemptive sjf: cpuBurstTime으로 정렬된 상태로 실행 가능한 프로세스만 레디큐 기입
        // 즉, preemptive sjf에서는 큐 맨 앞이 다음 실행할 프로세스
        Process* runProcess = peek(readyQueue);

        // I/O 처리
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        // Preemptive됐다는 표시
        // runProcess: 현재 실행 중인 프로세스
        // currentProcess: 이전에 실행한 프로세스
        if(currentProcess != NULL && runProcess->cpuBurstTime < currentProcess->cpuBurstTime){
            /** Ex
             * CPU Burst Time: A:4, B:2, C:3 일때
             * Process A: T3 ~ T5 실행 (remaining: 2)
             * Process B: T4 도착 (remaining: 2)
             * Process C: T5 ~ T8 실행 (remaining: 0)
             * T8에 실행할 프로세스는 A 혹은 B. 그러나 A실행 중에 B가 도착했기에 B가 실행되어야 한다 (longer waiting time)
             * 따라서, A가 preempted됐다면 B뒤로 재배치
             */
            reorderProcess(readyQueue, currentProcess, 1);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 1);

        // 이전 프로세스를 현재 프로세스로 할당
        currentProcess = runProcess;

        // 간트차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료
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

    // Priority 순으로 레디큐 기입 (타이브레이커: 도착순)
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
                // 실행 가능한 프로세스 여부 확인
                if (currentNode -> process -> arrivalTime <= timeUnit){
                    runProcess = currentNode -> process;
                    break;
                }
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 2);
            timeUnit++;
            continue;
        }

        // 프로세스 실행 중, 방해 금지
        if(deactivatePreeemptionCounter == 0){
            deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
        }else{
            deactivatePreeemptionCounter--;
        }

        // I/O 처리
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 2);

        // 간트차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료
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
        // Priority 순으로 레디큐 기입 (타이브레이커: 도착순)
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueuePriority(readyQueue, &processesCopy[i]);
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 2);
            timeUnit++;
            continue;
        }

        // 실행 가능한 프로세스 O, 큐 맨 앞 프로세스 실행
        Process* runProcess = peek(readyQueue);

        // I/O 처리
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeue(readyQueue);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        // Preemtive 됐으므로 재배치
        if(currentProcess != NULL && runProcess->priority < currentProcess->priority){
            reorderProcess(readyQueue, currentProcess, 2);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 2);

        currentProcess = runProcess;

        // 간트차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료
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
    // 남은 burst time이 타임 퀀텀보다 작을 수도 있어서 따로 변수로 관리
    int runTime = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while(n > 0){
        // enqueue: 도착순으로 레디큐에 프로세스 기입
        for(int i = 0; i < GLOBAL__PROCESS_COUNT; i++){
            if(processesCopy[i].arrivalTime == timeUnit){
                enqueue(readyQueue, &processesCopy[i]);
                continue;
            }
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(isEmpty(readyQueue)){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }
        
        // 실행 가능한 프로세스 O, 큐 맨 앞 프로세스 실행
        currentProcess = peek(readyQueue);
        
        // quantumCounter: 프로세스가 할당 받은 시간 동안에는 방해받지 않는다, non preemptive
        if (!quantumCounter) {
            dequeue(readyQueue);
            // 프로세스 종료
            if(currentProcess->cpuBurstTime == 0){
                currentProcess->completionTime = timeUnit;
                n--;
                // dequeue()를 여기서 하기에 스케쥴링 종료도 여기서 해주어야 한다
                // n: 살아있는 프로세스 개수
                if(n==0){
                    break;
                }
            } else { // 다시 레디큐 기입
                enqueue(readyQueue, currentProcess);
            }

            // 다음 프로세스 실행
            currentProcess = peek(readyQueue);
            // 남은 burst time이 타임 퀀텀보다 작은 경우
            runTime = (currentProcess->cpuBurstTime < quantum) ? currentProcess->cpuBurstTime : quantum;
            quantumCounter = --runTime;
        } else {
            quantumCounter--;
        }

        // 간트 차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, currentProcess->pid, timeUnit, timeUnit + 1);
        currentProcess->cpuBurstTime--;
        currentProcess->ioTime--;

        // I/O 처리
        if (currentProcess->ioTime == 0) {
            enqueue(waitingQueue, currentProcess);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        timeUnit++;
    }

    evaluate(processes, processesCopy, GLOBAL__PROCESS_COUNT);
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
        // HRRN은 nonpreemptive
        if(!deactivatePreeemptionCounter){
            // highestResponseRatio(): 현재 실행 가능한 프로세스 중에서 HRR 반환
            runProcess = highestResponseRatio(readyQueue, timeUnit);
            if(runProcess != NULL){
                deactivatePreeemptionCounter = runProcess->cpuBurstTime - 1;
            }
        } else {
            deactivatePreeemptionCounter--;
        }

        // 실행 가능한 프로세스 X, idle 처리
        if(runProcess == NULL){
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }

        // I/O 처리
        if(runProcess->ioTime == 0){
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            runProcess = peek(readyQueue);
        }

        // Waiting queue 처리
        executeWaitingQueue(waitingQueue, readyQueue, 0);

        // 간트차트 처리
        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // 프로세스 종료
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

void edfScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* processesCopy = copyProcesses(processes, n);

    Queue* readyQueue = createQueue();
    Queue* waitingQueue = createQueue();

    GanttProcess* ganttQueue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while (n > 0) {
        for (int i = 0; i < GLOBAL__PROCESS_COUNT; i++) {
            if (processesCopy[i].arrivalTime == timeUnit) {
                enqueue(readyQueue, &processesCopy[i]);
            }
        }

        if (isEmpty(readyQueue)) {
            enqueueGanttProcess(&ganttQueue, &queueCount, -1, timeUnit, timeUnit + 1);
            executeWaitingQueue(waitingQueue, readyQueue, 0);
            timeUnit++;
            continue;
        }

        // Find the process with the earliest deadline
        Node* currentNode = readyQueue->front;
        Process* runProcess = currentNode->process;
        while (currentNode != NULL) {
            if (currentNode->process->deadline < runProcess->deadline) {
                runProcess = currentNode->process;
            }
            currentNode = currentNode->next;
        }

        if (runProcess->ioTime == 0) {
            runProcess->ioTime--;
            dequeueByPid(readyQueue, runProcess->pid);
            enqueue(waitingQueue, runProcess);
            continue;
        }

        executeWaitingQueue(waitingQueue, readyQueue, 0);

        enqueueGanttProcess(&ganttQueue, &queueCount, runProcess->pid, timeUnit, timeUnit + 1);
        runProcess->cpuBurstTime--;
        runProcess->ioTime--;

        // Add 1 to waiting time for all processes that are not being processed
        for (currentNode = readyQueue->front; currentNode != NULL; currentNode = currentNode->next) {
            if (currentNode->process != runProcess) {
                currentNode->process->waitingTime++;
            }
        }

        if (runProcess->cpuBurstTime == 0) {
            runProcess->completionTime = timeUnit + 1;
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
