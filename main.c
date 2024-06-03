#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "createProcesses.h"
#include "utils.h"
#include "scheduling.h"
#include "evaluate.h"

int GLOBAL__MAX_PID = 1000;
int GLOBAL__MAX_CPU_BURST_TIME = 15;
int GLOBAL__MAX_IO_BURST_TIME = 10;
int GLOBAL__MAX_ARRIVAL_TIME = 20;
int GLOBAL__MAX_PRIORITY = 20;
int GLOBAL__PROCESS_COUNT = 5;

int main() {
    // 랜덤 시드값 (현재 시간으로)
    srand(time(NULL));

    int choice;
    printf("Choose process creation method:\n");
    printf("1. Random\n");
    printf("2. Custom\n");
    printf("3. Debug\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    Process* processes;
    if (choice == 1) {
        processes = createProcesses();
    } else if (choice == 2) {
        processes = createCustomProcesses();
    } else if (choice == 3) {
        processes = createDebugProcesses();
    } else {
        printf("Invalid choice.\n");
        return 1;
    }

    // Process* processes = createDebugProcesses();

    sortProcessesByArrivalTime(processes);

    evaluateAll(processes);

    free(processes);
    return 0;
}
