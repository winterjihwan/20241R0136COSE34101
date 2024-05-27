#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// 버블소트
void sortProcessesByArrivalTime(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void sortProcessesByCpuBurstTime(Process* processes, int start, int end) {
    for (int i = start; i < end - 1; i++) {
        for (int j = start; j < end - 1 - (i - start); j++) {
            if (processes[j].cpuBurstTime > processes[j + 1].cpuBurstTime) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void printProcesses(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;

    for (int i = 0; i < n; i++) {
        printf("Process ID: %d\n", processes[i].pid);
        printf("CPU Burst Time: %d\n", processes[i].cpuBurstTime);
        printf("I/O Burst Time: %d\n", processes[i].ioBurstTime);
        printf("Arrival Time: %d\n", processes[i].arrivalTime);
        printf("Priority: %d\n", processes[i].priority);
        printf("\n");
    }
}

int sumOfBurstTime(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += processes[i].cpuBurstTime;
    }
    return sum;
}

Process* copyProcesses(Process* original, int n) {
    Process* copy = (Process*)malloc(n * sizeof(Process));
    if (copy == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        copy[i] = original[i];
    }

    return copy;
}

void removeProcessByIndex(Process* processes, int* n, int index) {
    for (int i = index; i < *n - 1; ++i) {
        processes[i] = processes[i + 1];
    }
    (*n)--;
}
