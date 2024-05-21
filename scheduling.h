#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "process.h"

void fcfsScheduling(Process* processes);
void sjfScheduling(Process* processes);
void preemptiveSjfScheduling(Process* processes);

#endif // SCHEDULING_H