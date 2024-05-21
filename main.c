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

    Process* processes = createProcesses();

    printProcesses(processes);
    // preemptiveSjfScheduling(processes);

    free(processes);
    return 0;
}
