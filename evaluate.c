#include <stdio.h>
#include "evaluate.h"

void evaluate(Process *processes, Process *processesCopy, int processCount) {
    int totalTurnaroundTime = 0;
    int totalWaitingTime = 0;

    printf("|---------------------------------------------------------------------------------|\n");
    printf("| Job  | Arrival Time | Burst Time | Finish Time | Turnaround Time | Waiting Time |\n");
    printf("|------|--------------|------------|-------------|-----------------|--------------|\n");
    
    for (int i = 0; i < processCount; i++) {
        int turnaroundTime = processesCopy[i].completionTime - processes[i].arrivalTime;
        int waitingTime = turnaroundTime - processes[i].cpuBurstTime; 

        totalTurnaroundTime += turnaroundTime;
        totalWaitingTime += waitingTime;

        printf("| %-5d| %-13d| %-11d| %-12d| %-16d| %-12d |\n", 
                processes[i].pid,
                processes[i].arrivalTime, 
                processes[i].cpuBurstTime, 
                processesCopy[i].completionTime, 
                turnaroundTime, 
                waitingTime);
    }

    printf("|---------------------------------------------------------------------------------|\n");
    printf("Average Turnaround Time = %.2f\n", (float)totalTurnaroundTime / processCount);
    printf("Average Waiting Time = %.2f\n", (float)totalWaitingTime / processCount);
}
