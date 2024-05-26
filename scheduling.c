#include "scheduling.h"
#include "utils.h"
#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>

void fcfsScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    sortProcessesByArrivalTime(processes);

    GanttQueue *queue = malloc(sizeof(GanttQueue) + n * sizeof(PIDandTAIL));
    if (queue == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    queue->count = 0;

    int currentTime = 0;

    for (int i = 0; i < n; i++) {
        if (currentTime < processes[i].arrivalTime) {
            enqueueGanttQueue(&queue, -1, currentTime, processes[i].arrivalTime); // CPU is idle
            currentTime = processes[i].arrivalTime;
        }
        int start = currentTime;
        int end = start + processes[i].cpuBurstTime;
        enqueueGanttQueue(&queue, processes[i].pid, start, end);
        currentTime = end;
    }

    printGanttChart(queue);
    free(queue);
}

void sjfScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

    GanttQueue *queue = malloc(sizeof(GanttQueue) + n * sizeof(PIDandTAIL));
    if (queue == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    queue->count = 0;

    while (n > 0) {
        int shortestJobIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (shortestJobIndex == -1 || copyRQ[j].cpuBurstTime < copyRQ[shortestJobIndex].cpuBurstTime) {
                    shortestJobIndex = j;
                }
            }
        }

        if (shortestJobIndex == -1) {
            ++timeUnit;
            enqueueGanttQueue(&queue, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[shortestJobIndex];
        int startTime = timeUnit;
        int endTime = startTime + nextProcess.cpuBurstTime;
        enqueueGanttQueue(&queue, nextProcess.pid, startTime, endTime);
        timeUnit = endTime;

        removeProcessByIndex(copyRQ, &n, shortestJobIndex);
    }

    printGanttChart(queue);
    free(queue);
    free(copyRQ);
}


void preemptiveSjfScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;
    int totalBurstTime = sumOfBurstTime(processes);

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

    GanttQueue *queue = malloc(sizeof(GanttQueue) + totalBurstTime * sizeof(PIDandTAIL));
    if (queue == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    queue->count = 0;

    while (n > 0) {
        int shortestJobIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (shortestJobIndex == -1 || copyRQ[j].cpuBurstTime < copyRQ[shortestJobIndex].cpuBurstTime) {
                    shortestJobIndex = j;
                }
            }
        }

        if (shortestJobIndex == -1) {
            ++timeUnit;
            enqueueGanttQueue(&queue, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[shortestJobIndex];
        enqueueGanttQueue(&queue, nextProcess.pid, timeUnit, timeUnit + 1);
        copyRQ[shortestJobIndex].cpuBurstTime--;
        if (copyRQ[shortestJobIndex].cpuBurstTime == 0) {
            removeProcessByIndex(copyRQ, &n, shortestJobIndex);
        }

        timeUnit++;
    }

    printGanttChart(queue);
    free(queue);
    free(copyRQ);
}

void priorityScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

    GanttQueue *queue = malloc(sizeof(GanttQueue) + n * sizeof(PIDandTAIL));
    if (queue == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    queue->count = 0;

    while (n > 0) {
        int highestPriorityIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (highestPriorityIndex == -1 || copyRQ[j].priority <= copyRQ[highestPriorityIndex].priority) {
                    highestPriorityIndex = j;
                }
            }
        }

        if (highestPriorityIndex == -1) {
            ++timeUnit;
            enqueueGanttQueue(&queue, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[highestPriorityIndex];
        int startTime = timeUnit;
        int endTime = startTime + nextProcess.cpuBurstTime;
        enqueueGanttQueue(&queue, nextProcess.pid, startTime, endTime);
        timeUnit = endTime;

        removeProcessByIndex(copyRQ, &n, highestPriorityIndex);
    }

    printGanttChart(queue);
    free(queue);
    free(copyRQ);
}

void preemptivePriorityScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;
    int totalBurstTime = sumOfBurstTime(processes);

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

    GanttQueue *queue = malloc(sizeof(GanttQueue) + totalBurstTime * sizeof(PIDandTAIL));
    if (queue == NULL) {
        perror("Failed to allocate memory");
        exit(1);
    }
    queue->count = 0;

    while (n > 0) {
        int highestPriorityIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (highestPriorityIndex == -1 || copyRQ[j].priority < copyRQ[highestPriorityIndex].priority) {
                    highestPriorityIndex = j;
                }
            }
        }

        if (highestPriorityIndex == -1) {
            ++timeUnit;
            enqueueGanttQueue(&queue, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[highestPriorityIndex];
        enqueueGanttQueue(&queue, nextProcess.pid, timeUnit, timeUnit + 1);
        copyRQ[highestPriorityIndex].cpuBurstTime--;
        if (copyRQ[highestPriorityIndex].cpuBurstTime == 0) {
            removeProcessByIndex(copyRQ, &n, highestPriorityIndex);
        }

        timeUnit++;
    }

    printGanttChart(queue);
    free(queue);
    free(copyRQ);
}