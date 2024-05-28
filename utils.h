#ifndef UTILS_H
#define UTILS_H

#include "process.h"

void sortProcessesByArrivalTime(Process* processes);
void printProcesses(Process* processes);
Process* copyProcesses(Process* original, int n);
void removeProcessByIndex(Process* processes, int* n, int index);

#endif // UTILS_H
