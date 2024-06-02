#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

// 버블소트
void sortProcessesByArrivalTime(Process* processes) {
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

// 모든 프로세스 출력
void printProcesses(Process* processes) {
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

// 프로세스 sequence 딥커피
Process* copyProcesses(Process* original, int n) {
    // (프로세스 개수 n * 프로세스 구조체 사이즈)만큼 메모리 할당
    Process* copy = (Process*)malloc(n * sizeof(Process));
    if (copy == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 딥커피
    for (int i = 0; i < n; i++) {
        copy[i] = original[i];
    }

    return copy;
}

// 프로세스 index로 삭제
// Deprecated
void removeProcessByIndex(Process* processes, int* n, int index) {
    // [0, 3, 6, 9] 배열의 1번 인덱스 삭제 -> [0, 6, 9, 9]
    // index 이후의 모든 프로세스를 한 칸씩 앞으로 당기고 n을 1 감소시킨다
    for (int i = index; i < *n - 1; ++i) {
        processes[i] = processes[i + 1];
    }
    // 스케듈러의 프로세스 개수 감소 -> 0이 되면 스케줄러 종료
    (*n)--;
}
