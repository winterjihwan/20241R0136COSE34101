#include "createProcesses.h"
#include <stdlib.h>
#include <stdbool.h>

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
