#include "scheduling.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void fcfsScheduling(Process* processes) {
    sortProcessesByArrivalTime(processes);

    int n = GLOBAL__PROCESS_COUNT;
    int currentTime = 0;

    printf("Gantt Chart:\n");
    printf("-------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        int start = currentTime;
        int end = start + processes[i].cpuBurstTime;

        printf("| P%d (%d - %d) ", processes[i].pid, start, end);

        currentTime = end;
    }

    printf("|\n-------------------------------------------------\n");
}

void sjfScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

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
            printf("time elapsed, no process executed in this time unit\n");
            continue;
        }

        Process nextProcess = copyRQ[shortestJobIndex];
        timeUnit += nextProcess.cpuBurstTime;

        printf("Process %d executed from time %d to %d\n",
               nextProcess.pid, timeUnit - nextProcess.cpuBurstTime, timeUnit);

        removeProcessByIndex(copyRQ, &n, shortestJobIndex);
    }

    free(copyRQ);
}

void preemptiveSjfScheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;
    int totalBurstTime = sumOfBurstTime(processes);

    Process* copyRQ = copyProcesses(processes, n);

    sortProcessesByArrivalTime(copyRQ);

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
            printf("time elapsed, no process executed in this time unit\n");
            continue;
        }

        Process nextProcess = copyRQ[shortestJobIndex];
        copyRQ[shortestJobIndex].cpuBurstTime --;
        if (copyRQ[shortestJobIndex].cpuBurstTime == 0) {
            removeProcessByIndex(copyRQ, &n, shortestJobIndex);
        }

        timeUnit ++;

        printf("Process %d executed from time %d to %d\n",
               nextProcess.pid, timeUnit - 1, timeUnit);
    }

    free(copyRQ);
}
