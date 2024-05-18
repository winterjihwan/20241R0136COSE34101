#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int global_pid = 1;

typedef struct {
    int pid;         
    int cpuBurstTime;   
    int ioBurstTime;   
    int arrivalTime; 
    int priority;    
} Process;

Process Create_Process(int pid) {
    Process p;
    p.pid = pid;
    p.cpuBurstTime = rand() % 100 + 1;   
    p.ioBurstTime = rand() % 50 + 1;     
    p.arrivalTime = rand() % 100;        
    p.priority = rand() % 10 + 1;        
    return p;
}


int main() {
    Process p = Create_Process(1);
    printf("Process ID: %d\n", p.pid);
    printf("CPU Burst Time: %d\n", p.cpuBurstTime);
    printf("I/O Burst Time: %d\n", p.ioBurstTime);
    printf("Arrival Time: %d\n", p.arrivalTime);
    printf("Priority: %d\n", p.priority);
    return 0;
}