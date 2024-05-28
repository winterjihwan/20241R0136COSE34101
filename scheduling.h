#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "process.h"

void fcfsScheduling(Process* processes);
void sjfScheduling(Process* processes);
void preemptiveSjfScheduling(Process* processes);
void priorityScheduling(Process* processes);
void preemptivePriorityScheduling(Process* processes);
void roundRobinScheduling(Process* processes, int quantum);
void hrrnScheduling(Process* processes);

#endif // SCHEDULING_H
