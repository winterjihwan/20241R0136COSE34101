#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>

// 다이너믹 메모리 할당
void enqueueGanttProcess(GanttProcess **queue, int *count, int pid, int startTime, int endTime) {
    int newCount = *count + 1;
    // realloc 함수로 큐의 크기를 1 증가
    *queue = realloc(*queue, newCount * sizeof(GanttProcess));
    if (*queue == NULL) {
        perror("Failed to reallocate memory");
        exit(1);
    }

    // 큐에 프로세스 정보 추가
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

        // 같은 프로세스가 연속으로 실행되는 경우, idle 포함
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
    printf("|\n\n\n\n\n\n");
}


