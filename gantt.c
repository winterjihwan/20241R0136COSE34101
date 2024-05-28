#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>

void enqueueGanttProcess(GanttProcess **queue, int *count, int pid, int startTime, int endTime) {
    int newCount = *count + 1;
    *queue = realloc(*queue, newCount * sizeof(GanttProcess));
    if (*queue == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }

    (*queue)[*count].pid = pid;
    (*queue)[*count].startTime = startTime;
    (*queue)[*count].endTime = endTime;
    *count = newCount;
}

void printGanttChart(GanttProcess *queue, int count) {
    for (int i = 0; i < count; i++) {
        int pid = queue[i].pid;
        int start = queue[i].startTime;
        int end = queue[i].endTime;

        while (i < count - 1 && queue[i + 1].pid == pid && queue[i + 1].startTime == end) {
            end = queue[++i].endTime;
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


