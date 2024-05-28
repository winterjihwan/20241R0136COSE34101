#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "createProcesses.h"
#include "utils.h"
#include "scheduling.h"

int GLOBAL__MAX_PID = 1000;
int GLOBAL__MAX_CPU_BURST_TIME = 15;
int GLOBAL__MAX_IO_BURST_TIME = 10;
int GLOBAL__MAX_ARRIVAL_TIME = 20;
int GLOBAL__MAX_PRIORITY = 20;
int GLOBAL__PROCESS_COUNT = 5;

int main() {
    srand(time(NULL));

    int choice;
    // printf("Choose process creation method:\n");
    // printf("1. Random\n");
    // printf("2. Custom\n");
    // printf("Enter your choice: ");
    // scanf("%d", &choice);

    Process* processes;
    // if (choice == 1) {
    //     processes = createProcesses();
    // } else if (choice == 2) {
    //     processes = createCustomProcesses();
    // } else {
    //     printf("Invalid choice.\n");
    //     return 1;
    // }


    processes = createDebugProcesses();
    printProcesses(processes);
    sortProcessesByArrivalTime(processes);

    // printf("FCFS Scheduling\n");
    // fcfsScheduling(processes);
    // printf("SJF Scheduling\n");
    // sjfScheduling(processes);
    // printf("Preemptive SJF Scheduling\n");
    // preemptiveSjfScheduling(processes);
    // printf("Priority Scheduling\n");
    // priorityScheduling(processes);
    // printf("Preemptive Priority Scheduling\n");
    // preemptivePriorityScheduling(processes);
    printf("Round Robin Scheduling\n");
    roundRobinScheduling(processes, 3);

    free(processes);
    return 0;
}
