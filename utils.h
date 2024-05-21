#ifndef UTILS_H
#define UTILS_H

#include "process.h"

void sortProcessesByArrivalTime(Process* processes);
void sortProcessesByCpuBurstTime(Process* processes, int start, int end);
void printProcesses(Process* processes);
int sumOfBurstTime(Process* processes);
Process* copyProcesses(Process* original, int n);
void removeProcessByIndex(Process* processes, int* n, int index);

#endif // UTILS_H
