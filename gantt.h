#ifndef GANTT_H
#define GANTT_H

typedef struct {
    int pid;
    int startTime;
    int endTime;
} PIDandTAIL;

typedef struct {
    int count;
    PIDandTAIL ganntQueuePIDS[];
} GanttQueue;

void enqueueGanttQueue(GanttQueue **queue, int pid, int startTime, int endTime);
void printGanttChart(GanttQueue *queue);

#endif // GANTT_H
