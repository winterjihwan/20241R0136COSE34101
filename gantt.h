#ifndef GANTT_H
#define GANTT_H

typedef struct {
    int pid;
    int startTime;
    int endTime;
} GanttProcess;


void enqueueGanttProcess(GanttProcess **queue, int *count, int pid, int startTime, int endTime);
void printGanttChart(GanttProcess *queue, int count);

#endif // GANTT_H
