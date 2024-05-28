#include "createProcesses.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

int randomPid(int usedPids[], int count) {
    int pid;
    bool unique;
    do {
        pid = rand() % GLOBAL__MAX_PID;
        unique = true;
        for (int i = 0; i < count; i++) {
            if (usedPids[i] == pid) {
                unique = false;
                break;
            }
        }
    } while (!unique);
    return pid;
}

Process createProcess(int pid) {
    Process p;
    p.pid = pid;
    p.cpuBurstTime = rand() % GLOBAL__MAX_CPU_BURST_TIME + 1;   
    p.ioBurstTime = rand() % GLOBAL__MAX_IO_BURST_TIME + 1;     
    p.arrivalTime = rand() % GLOBAL__MAX_ARRIVAL_TIME;        
    p.priority = rand() % GLOBAL__MAX_PRIORITY;        
    return p;
}

Process* createProcesses() {
    int n = GLOBAL__PROCESS_COUNT;
    Process* processes = (Process*)malloc(n * sizeof(Process));
    int* usedPids = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int pid = randomPid(usedPids, i);
        usedPids[i] = pid;
        processes[i] = createProcess(pid);
    }

    free(usedPids);
    return processes;
}

Process* createCustomProcesses() {
    int n = GLOBAL__PROCESS_COUNT;
    Process* processes = (Process*)malloc(n * sizeof(Process));
    if (!processes) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        printf("Enter details for process %d:\n", i + 1);

        printf("PID: ");
        scanf("%d", &processes[i].pid);

        printf("CPU Burst Time: ");
        scanf("%d", &processes[i].cpuBurstTime);

        printf("I/O Burst Time: ");
        scanf("%d", &processes[i].ioBurstTime);

        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrivalTime);

        printf("Priority: ");
        scanf("%d", &processes[i].priority);

        printf("\n");
    }

    return processes;
}

Process* createDebugProcesses() {
    int n = GLOBAL__PROCESS_COUNT;
    Process* processes = (Process*)malloc(n * sizeof(Process));
    if (!processes) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // processes[0].pid = 1;
    // processes[0].arrivalTime = 3;
    // processes[0].cpuBurstTime = 10;
    // processes[0].priority = 1;
    // processes[0].ioTime = -1;
    // processes[0].waitingTime = 0;

    // processes[1].pid = 2;
    // processes[1].arrivalTime = 1;
    // processes[1].cpuBurstTime = 5;
    // processes[1].priority = 2;
    // processes[1].ioTime = -1;
    // processes[1].waitingTime = 0;


    // processes[2].pid = 3;
    // processes[2].arrivalTime = 2;
    // processes[2].cpuBurstTime = 3;
    // processes[2].priority = 3;
    // processes[2].ioBurstTime = 5;
    // processes[2].ioTime = -1;
    // processes[2].waitingTime = 0;


    // processes[3].pid = 4;
    // processes[3].arrivalTime = 5;
    // processes[3].cpuBurstTime = 2;
    // processes[3].priority = 4;
    // processes[3].ioTime = -1;
    // processes[3].waitingTime = 0;


    // processes[4].pid = 5;
    // processes[4].arrivalTime = 5;
    // processes[4].cpuBurstTime = 8;
    // processes[4].priority = 5;
    // processes[4].ioTime = -1;
    // processes[4].waitingTime = 0;

    processes[0].pid = 1;
    processes[0].arrivalTime = 1;
    processes[0].cpuBurstTime = 3;
    processes[0].priority = 1;
    processes[0].ioTime = -1;
    processes[0].waitingTime = 0;

    processes[1].pid = 2;
    processes[1].arrivalTime = 3;
    processes[1].cpuBurstTime = 6;
    processes[1].priority = 2;
    processes[1].ioTime = -1;
    processes[1].waitingTime = 0;


    processes[2].pid = 3;
    processes[2].arrivalTime = 5;
    processes[2].cpuBurstTime = 8;
    processes[2].priority = 3;
    processes[2].ioBurstTime = 5;
    processes[2].ioTime = -1;
    processes[2].waitingTime = 0;


    processes[3].pid = 4;
    processes[3].arrivalTime = 7;
    processes[3].cpuBurstTime = 4;
    processes[3].priority = 4;
    processes[3].ioTime = -1;
    processes[3].waitingTime = 0;


    processes[4].pid = 5;
    processes[4].arrivalTime = 8;
    processes[4].cpuBurstTime = 5;
    processes[4].priority = 5;
    processes[4].ioTime = -1;
    processes[4].waitingTime = 0;


    return processes;
}