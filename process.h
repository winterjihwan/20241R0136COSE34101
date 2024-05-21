#ifndef PROCESS_H
#define PROCESS_H

extern int GLOBAL__MAX_PID;
extern int GLOBAL__MAX_CPU_BURST_TIME;
extern int GLOBAL__MAX_IO_BURST_TIME;
extern int GLOBAL__MAX_ARRIVAL_TIME;
extern int GLOBAL__MAX_PRIORITY;
extern int GLOBAL__PROCESS_COUNT;

typedef struct {
    int pid;            
    int cpuBurstTime;   
    int ioBurstTime;    
    int arrivalTime;    
    int priority;       
} Process;

#endif // PROCESS_H
