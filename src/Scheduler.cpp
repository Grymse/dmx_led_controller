#include <Arduino.h>
#include <vector>

class Task {
  public:
    virtual void run() = 0;
};

struct Process {
  long currentTick;
  long nextTickMillis;
  int tickInterval;
  Task* task;
};

class Scheduler {
  public:
    std::vector<Process*> processes;

    void addProcess(Process* process) {
        processes.push_back(process);
    }

    void run() {
        for (Process* process : processes) {
            long currentMillis = millis();
            if (currentMillis >= process->nextTickMillis) {
                process->currentTick++;
                process->nextTickMillis = currentMillis + process->tickInterval;
            } else continue;

            process->task->run();
        }
    }
};
