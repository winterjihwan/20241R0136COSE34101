#include "scheduling.h"
#include "utils.h"
#include "gantt.h"
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/**
 * @brief FCFS 스케줄링
 * @details 도착 순으로 정렬되어 있는 프로세스 시퀀스를 인자로 받았다는  것을 전제한다 (main()에서 sortProcessesByArrivalTime() 호출)
 * @param processes 프로세스 배열
 */
void fcfsScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    // 프로세스 개수
    int n = GLOBAL__PROCESS_COUNT;
    // 출력을 위해 갠트차트 initialize
    GanttProcess *queue = NULL;
    int queueCount = 0;
    // Time 0으로 초기화
    int timeUnit = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    // 이미 도착 시간순으로 정령되어있지만, 도착 시간이 0이 아닌 경우를 고려한다
    for (int i = 0; i < n; i++) {
        if (timeUnit < processes[i].arrivalTime) {
            // 다음 실행 프로세스의 도착 시간이 timeUnit보다 클 경우 => 지금 실행 가능한 프로세스가 없다; 갠트 큐에 idle 기입
            enqueueGanttProcess(&queue, &queueCount, -1, timeUnit, processes[i].arrivalTime);
            // 다음 실행 프로세스 시작 시간으로 설정
            timeUnit = processes[i].arrivalTime;
        }
        // 현재 프로세스 처리
        int start = timeUnit;
        int end = start + processes[i].cpuBurstTime;
        enqueueGanttProcess(&queue, &queueCount, processes[i].pid, start, end);
        timeUnit = end;
    }

    // 갠트차트 출력
    printGanttChart(queue, queueCount);
    free(queue);
}

void sjfScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    // 프로세스 개수
    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    // Processes 재사용을 위해 레디큐 복사
    Process* copyRQ = copyProcesses(processes, n);

    // 출력을 위해 갠트 차트 initialize
    GanttProcess *queue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    // FCFS와는 달리, 프로세스 수만큼 루프하는 것이 아니라, 모든 프로세스가 작업을 마칠 때까지 루프
    while (n > 0) {
        // [SJI] 가장 짧은 CPU Burst Time을 가진 프로세스의 인덱스; -1일 경우 실행 가능한 프로세스가 없다 
        int shortestJobIndex = -1;
        // 다음 실행 후보를 선택 하기 위한 for loop
        for (int j = 0; j < n; j++) {
            // 현재 실행 가능한 프로세스
            if (copyRQ[j].arrivalTime <= timeUnit) {
                // 처음으로 배정 가능한 프로세스
                if (shortestJobIndex == -1) {
                    shortestJobIndex = j;
                    // burst time이 더 짧은 프로세스
                } else if (copyRQ[j].cpuBurstTime < copyRQ[shortestJobIndex].cpuBurstTime) {
                    shortestJobIndex = j;
                    // 일종의 타이브레이커, 동일한 burst time의 경우 arrival time으로 비교
                } else if (copyRQ[j].cpuBurstTime == copyRQ[shortestJobIndex].cpuBurstTime && copyRQ[j].arrivalTime < copyRQ[shortestJobIndex].arrivalTime) {
                    shortestJobIndex = j;
                }
            }
        }

        // 프로세스 전부 탐색했는데, 실행 가능한 프로세스가 없다면 idle기입
        if (shortestJobIndex == -1) {
            ++timeUnit;
            enqueueGanttProcess(&queue, &queueCount, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        // 실행 가능한 프로세스가 있다면, 해당 프로세스를 처리
        Process nextProcess = copyRQ[shortestJobIndex];
        int startTime = timeUnit;
        // non preemptive이므로, burst time만큼 방해 없이 실행
        int endTime = startTime + nextProcess.cpuBurstTime;
        enqueueGanttProcess(&queue, &queueCount, nextProcess.pid, startTime, endTime);
        timeUnit = endTime;
        // 프로세스 처리 완료했다면 레디큐에서 릴리즈
        removeProcessByIndex(copyRQ, &n, shortestJobIndex);
    }

    // 갠트차트 출력
    printGanttChart(queue, queueCount);
    free(queue);
    free(copyRQ);
}

void preemptiveSjfScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);
    GanttProcess *queue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while (n > 0) {
        // [SJI] 현재 실행 가능한 프로세스 중 가장 짧은 burst time을 찾는 과정은 동일하다
        int shortestJobIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (shortestJobIndex == -1) {
                    shortestJobIndex = j;
                } else if (copyRQ[j].cpuBurstTime < copyRQ[shortestJobIndex].cpuBurstTime) {
                    shortestJobIndex = j;
                } else if (copyRQ[j].cpuBurstTime == copyRQ[shortestJobIndex].cpuBurstTime && copyRQ[j].arrivalTime < copyRQ[shortestJobIndex].arrivalTime) {
                    shortestJobIndex = j;
                }
            }
        }

        // 실행 가능한 프로세스가 없다면 idle 기입
        if (shortestJobIndex == -1) {
            ++timeUnit;
            enqueueGanttProcess(&queue, &queueCount, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[shortestJobIndex];
        // non preemptive sjf와 다르게, burst time 전부 처리하는 것이 아닌, 1 time unit만큼만 처리
        enqueueGanttProcess(&queue, &queueCount, nextProcess.pid, timeUnit, timeUnit + 1);
        // 레디큐 릴리즈 대신 1 time unit만큼 burst time 감소
        copyRQ[shortestJobIndex].cpuBurstTime--;
        // burst time이 0이 되었다면 레디큐에서 릴리즈
        if (copyRQ[shortestJobIndex].cpuBurstTime == 0) {
            removeProcessByIndex(copyRQ, &n, shortestJobIndex);
        }
        // preemption 발생 확인은 매초 한다
        timeUnit++;
    }

    printGanttChart(queue, queueCount);
    free(queue);
    free(copyRQ);
}

void priorityScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);
    GanttProcess *queue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while (n > 0) {
        // [HPI] 가장 높은 (수치로 낮은) priority를 가진 프로세스의 인덱스; -1일 경우 실행 가능한 프로세스가 없다
        int highestPriorityIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (highestPriorityIndex == -1) {
                    highestPriorityIndex = j;
                    // priority가 더 높은 프로세스 
                } else if (copyRQ[j].priority < copyRQ[highestPriorityIndex].priority) {
                    highestPriorityIndex = j;
                    // 동일한 priority의 경우 arrival time으로 비교
                } else if (copyRQ[j].priority == copyRQ[highestPriorityIndex].priority && copyRQ[j].arrivalTime < copyRQ[highestPriorityIndex].arrivalTime) {
                    highestPriorityIndex = j;
                }
            }
        }

        // 실행 가능한 프로세스가 없다면 idle 기입
        if (highestPriorityIndex == -1) {
            ++timeUnit;
            enqueueGanttProcess(&queue, &queueCount, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[highestPriorityIndex];
        int startTime = timeUnit;
        // non preemptive이므로, burst time만큼 방해 없이 실행
        int endTime = startTime + nextProcess.cpuBurstTime;
        enqueueGanttProcess(&queue, &queueCount, nextProcess.pid, startTime, endTime);
        timeUnit = endTime;

        removeProcessByIndex(copyRQ, &n, highestPriorityIndex);
    }

    printGanttChart(queue, queueCount);
    free(queue);
    free(copyRQ);
}

void preemptivePriorityScheduling(Process* processes) {
    // ================================================================
    // │                            Setup                             │
    // ================================================================

    int n = GLOBAL__PROCESS_COUNT;
    int timeUnit = 0;

    Process* copyRQ = copyProcesses(processes, n);
    GanttProcess *queue = NULL;
    int queueCount = 0;

    // ================================================================
    // │                          Scheduling                          │  
    // ================================================================

    while (n > 0) {
        int highestPriorityIndex = -1;
        for (int j = 0; j < n; j++) {
            if (copyRQ[j].arrivalTime <= timeUnit) {
                if (highestPriorityIndex == -1) {
                    highestPriorityIndex = j;
                } else if (copyRQ[j].priority < copyRQ[highestPriorityIndex].priority) {
                    highestPriorityIndex = j;
                } else if (copyRQ[j].priority == copyRQ[highestPriorityIndex].priority) {
                    if(copyRQ[j].arrivalTime < copyRQ[highestPriorityIndex].arrivalTime){
                        highestPriorityIndex = j;
                    }
                }
            }
        }

        if (highestPriorityIndex == -1) {
            ++timeUnit;
            enqueueGanttProcess(&queue, &queueCount, -1, timeUnit - 1, timeUnit); // CPU is idle
            continue;
        }

        Process nextProcess = copyRQ[highestPriorityIndex];
        // preemptive이므로 1 time unit만큼만 처리
        enqueueGanttProcess(&queue, &queueCount, nextProcess.pid, timeUnit, timeUnit + 1);
        copyRQ[highestPriorityIndex].cpuBurstTime--;
        if (copyRQ[highestPriorityIndex].cpuBurstTime == 0) {
            removeProcessByIndex(copyRQ, &n, highestPriorityIndex);
        }

        timeUnit++;
    }

    printGanttChart(queue, queueCount);
    free(queue);
    free(copyRQ);
}