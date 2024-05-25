#include "scheduling.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int startTime;
    int endTime;
} PIDandTAIL;

typedef struct {
    int count;
    PIDandTAIL ganntQueuePIDS[];
} GanttQueue;

void enqueueGanttQueue(GanttQueue **queue, int pid, int startTime, int endTime) {
    int newCount = (*queue)->count + 1;
    *queue = realloc(*queue, sizeof(GanttQueue) + newCount * sizeof(PIDandTAIL));
    if (*queue == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }

    (*queue)->ganntQueuePIDS[(*queue)->count].pid = pid;
    (*queue)->ganntQueuePIDS[(*queue)->count].startTime = startTime;
    (*queue)->ganntQueuePIDS[(*queue)->count].endTime = endTime;
    (*queue)->count = newCount;
}


void printGanttChart(GanttQueue *queue) {
    printf("Gantt Chart:\n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < queue->count; i++) {
        int pid = queue->ganntQueuePIDS[i].pid;
        int start = queue->ganntQueuePIDS[i].startTime;
        int end = queue->ganntQueuePIDS[i].endTime;

        // Combine consecutive executions of the same process
        while (i < queue->count - 1 && queue->ganntQueuePIDS[i + 1].pid == pid && queue->ganntQueuePIDS[i + 1].startTime == end) {
            end = queue->ganntQueuePIDS[++i].endTime;
        }

        if (pid == -1) {
            printf("| Idle (%d - %d) ", start, end);
        } else {
            int duration = end - start;
            printf("| P%d (%d - %d) ", pid, start, end);

            // Adjust the length of the process display
            for (int j = 0; j < duration - 1; j++) {
                printf("-");
            }
        }
    }

    printf("\n-------------------------------------------------\n");
}

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
