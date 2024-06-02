#include <stdio.h>
#include <string.h>
#include "evaluate.h"
#include "scheduling.h"

float lowestTurnaroundTime = 9999999;
float lowestWaitingTime = 9999999;
char lowestTurnaroundTimeAlgorithm[10];
char lowestWaitingTimeAlgorithm[10];

void evaluateAll(Process *processes) {
    printf("FCFS Scheduling\n");
    fcfsScheduling(processes);
    printf("SJF Scheduling\n");
    sjfScheduling(processes);
    printf("Preemptive SJF Scheduling\n");
    preemptiveSjfScheduling(processes);
    printf("Priority Scheduling\n");
    priorityScheduling(processes);
    printf("Preemptive Priority Scheduling\n");
    preemptivePriorityScheduling(processes);
    printf("Round Robin Scheduling\n");
    roundRobinScheduling(processes, 3);
    printf("HRRN Scheduling\n");
    hrrnScheduling(processes);

    printf("Lowest Turnaround Time: %.2f by %s\n", lowestTurnaroundTime, lowestTurnaroundTimeAlgorithm);
    printf("Lowest Waiting Time: %.2f by %s\n", lowestWaitingTime, lowestWaitingTimeAlgorithm);
}

void evaluate(Process *processes, Process *processesCopy, int processCount, char algorithm[10]) {
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;

    printf("|------------------------------------------------------------------------------------------------------------------------|\n");
    printf("| Job  | Arrival Time | Burst Time | I/O Burst Time | I/O Time | Priority | Finish Time | Turnaround Time | Waiting Time |\n");
    printf("|------|--------------|------------|----------------|----------|----------|-------------|-----------------|--------------|\n");

    for (int i = 0; i < processCount; i++) {
        int turnaroundTime = processesCopy[i].completionTime - processes[i].arrivalTime;
        int waitingTime = turnaroundTime - processes[i].cpuBurstTime;

        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        printf("| %-5d| %-13d| %-11d| %-15d| %-9d| %-9d| %-12d| %-16d| %-12d |\n", 
                processes[i].pid,
                processes[i].arrivalTime, 
                processes[i].cpuBurstTime, 
                processes[i].ioBurstTime,
                processes[i].ioTime,
                processes[i].priority,
                processesCopy[i].completionTime, 
                turnaroundTime, 
                waitingTime);
    }

    printf("|------------------------------------------------------------------------------------------------------------------------|\n");
    printf("Average Turnaround Time = %.2f\n", (float)totalTurnaroundTime / processCount);
    printf("Average Waiting Time = %.2f\n", (float)totalWaitingTime / processCount);

    if((float)totalTurnaroundTime / processCount < lowestTurnaroundTime) {
        lowestTurnaroundTime = (float)totalTurnaroundTime / processCount;
        strcpy(lowestTurnaroundTimeAlgorithm, algorithm);
    }
    if((float)totalWaitingTime / processCount < lowestWaitingTime) {
        lowestWaitingTime = (float)totalWaitingTime / processCount;
        strcpy(lowestWaitingTimeAlgorithm, algorithm);
    }
}
