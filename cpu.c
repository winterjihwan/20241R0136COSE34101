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
    p.cpuBurstTime = rand() % GLOBAL__MAX_CPU_BURST_TIME + 1;   
    p.ioBurstTime = rand() % GLOBAL__MAX_IO_BURST_TIME + 1;     
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

void sort_processes_by_arrival_time(Process* processes) {
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

void sort_processes_by_cpu_burst_time(Process* processes, int start, int end) {
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

void print_gantt_chart(Process* processes) {
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


void fcfs_scheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;

    sort_processes_by_arrival_time(processes);

    printf("FCFS Scheduling:\n");
    print_processes(processes);

    print_gantt_chart(processes);
}

int sum_of_burst_time(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += processes[i].cpuBurstTime;
    }
    return sum;
}

Process* copy_processes(Process* original, int n) {
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

void remove_process_by_index(Process* processes, int* n, int index) {
    for (int i = index; i < *n - 1; ++i) {
        processes[i] = processes[i + 1];
    }
    (*n)--;
}

void sjf_scheduling(Process* processes) {
    int n = GLOBAL__PROCESS_COUNT;
    int time_unit = 0;

    Process* copyRQ = copy_processes(processes, n);

    sort_processes_by_arrival_time(copyRQ);

    while (n > 0) {
        int shortest_job_index = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= time_unit) {
                if (shortest_job_index == -1 || copyRQ[j].cpuBurstTime < copyRQ[shortest_job_index].cpuBurstTime) {
                    shortest_job_index = j;
                }
            }
        }

        if (shortest_job_index == -1) {
            ++time_unit;
            printf("time elapsed, no process executed in this time unit");
            continue;
        }

        Process next_process = copyRQ[shortest_job_index];
        time_unit += next_process.cpuBurstTime;

        printf("Process %d executed from time %d to %d\n",
               next_process.pid, time_unit - next_process.cpuBurstTime, time_unit);

        remove_process_by_index(copyRQ, &n, shortest_job_index);
    }

    free(copyRQ);
}

void preemptive_sjf_scheduling(Process* processes){
    int n = GLOBAL__PROCESS_COUNT;
    int time_unit = 0;
    int total_burst_time = sum_of_burst_time(processes);

    Process* copyRQ = copy_processes(processes, n);

    sort_processes_by_arrival_time(copyRQ);

    while (n > 0) {
        int shortest_job_index = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= time_unit) {
                if (shortest_job_index == -1 || copyRQ[j].cpuBurstTime < copyRQ[shortest_job_index].cpuBurstTime) {
                    shortest_job_index = j;
                }
            }
        }

        if (shortest_job_index == -1) {
            ++time_unit;
            printf("time elapsed, no process executed in this time unit");
            continue;
        }

        Process next_process = copyRQ[shortest_job_index];
        copyRQ[shortest_job_index].cpuBurstTime --;
        if (copyRQ[shortest_job_index].cpuBurstTime == 0) {
            remove_process_by_index(copyRQ, &n, shortest_job_index);
        }
        time_unit ++;

        printf("Process %d executed from time %d to %d\n",
               next_process.pid, time_unit - 1, time_unit);
    }

    free(copyRQ);
}

int main() {
    srand(time(NULL));

    Process* processes = create_processes();

    // fcfs_scheduling(processes);
    print_processes(processes);
    // sjf_scheduling(processes);
    preemptive_sjf_scheduling(processes);

    free(processes);
    return 0;
}

