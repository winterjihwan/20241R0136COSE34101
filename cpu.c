#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int GLOBAL__MAX_PID = 1000;
int GLOBAL__MAX_CPU_BURST_TIME = 15;
int GLOBAL__MAX_IO_BURST_TIME = 10;
int GLOBAL__MAX_ARRIVAL_TIME = 20;
int GLOBAL__MAX_PRIORITY = 20;
int GLOBAL__PROCESS_COUNT = 5;

typedef struct {
    int pid;            
    int cpuBurstTime;   
    int ioBurstTime;    
    int arrivalTime;    
    int priority;       
} Process;

int random_pid(int used_pids[], int count) {
    int pid;
    bool unique;
    do {
        pid = rand() % GLOBAL__MAX_PID;
        unique = true;
        for (int i = 0; i < count; i++) {
            if (used_pids[i] == pid) {
                unique = false;
                break;
            }
        }
    } while (!unique);
    return pid;
}

Process create_process(int pid) {
    Process p;
    p.pid = pid;
    p.cpuBurstTime = rand() % GLOBAL__MAX_CPU_BURST_TIME;   
    p.ioBurstTime = rand() % GLOBAL__MAX_IO_BURST_TIME;     
    p.arrivalTime = rand() % GLOBAL__MAX_ARRIVAL_TIME;        
    p.priority = rand() % GLOBAL__MAX_PRIORITY;        
    return p;
}

Process* create_processes() {
    int n = GLOBAL__PROCESS_COUNT;
    Process* processes = (Process*)malloc(n * sizeof(Process));
    int* used_pids = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int pid = random_pid(used_pids, i);
        used_pids[i] = pid;
        processes[i] = create_process(pid);
    }

    free(used_pids);
    return processes;
}

void print_processes(Process* processes) {
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

void sort_processes_by_arrival_time(Process* processes, int n) {
    // Bubble Sort to sort processes by arrival time
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

void fcfs_scheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;

    // Sort processes by arrival time
    sort_processes_by_arrival_time(processes, n);

    // Print the sorted processes
    printf("FCFS Scheduling:\n");
    print_processes(processes);
}


int main() {
    srand(time(NULL));

    Process* processes = create_processes();

    fcfs_scheduling(processes);

    free(processes);
    return 0;
}

