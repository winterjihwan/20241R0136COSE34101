#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>

void enqueueGanttQueue(GanttQueue **queue, int pid, int startTime, int endTime) {
    int newCount = (*queue)->count + 1;
    *queue = realloc(*queue, sizeof(GanttQueue) + newCount * sizeof(PIDandTAIL));
    if (*queue == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }

    (*queue)->ganntQueuePIDS[(*queue)->count].pid = pid;
    (*queue)->ganntQueuePIDS[(*queue)->count].startTime = startTime;
    (*queue)->ganntQueuePIDS[(*queue)->count].endTime = endTime;
    (*queue)->count = newCount;
}

void printGanttChart(GanttQueue *queue) {
    for (int i = 0; i < queue->count; i++) {
        int pid = queue->ganntQueuePIDS[i].pid;
        int start = queue->ganntQueuePIDS[i].startTime;
        int end = queue->ganntQueuePIDS[i].endTime;

        while (i < queue->count - 1 && queue->ganntQueuePIDS[i + 1].pid == pid && queue->ganntQueuePIDS[i + 1].startTime == end) {
            end = queue->ganntQueuePIDS[++i].endTime;
        }

        if (pid == -1) {
            printf("| Idle (%d - %d) ", start, end);
        } else {
            int duration = end - start;
            printf("| P%d (%d - %d) ", pid, start, end);
            for (int j = 0; j < duration - 1; j++) {
                printf(" ");
            }
        }
    }
    printf("|\n");
}
