#include <Arduino.h>
#include <vector>
#include "process.h"
#include "../debug.cpp"

struct ScheduledProcess {
  unsigned long nextTickMillis;
  int tickInterval;
  Process* process;
};

class Scheduler {
  public:
    std::vector<ScheduledProcess*> processes;

    void addProcess(Process* process, int tickInterval) {

        processes.push_back(new ScheduledProcess{millis(), tickInterval, process});
    }

    void update() {
        
        for (ScheduledProcess* process : processes) {
            long currentMillis = millis();
            if (currentMillis < process->nextTickMillis) continue;
            
            process->nextTickMillis = currentMillis + process->tickInterval;
            long t = millis();
            process->process->update();
            debug("t%d\n",millis() - t);
        }
    }
};