#include <Arduino.h>
#include <vector>
#include "process.h"
#include "debug.h"

/**
 * @brief A struct to represent a scheduled process
 *
 */
struct ScheduledProcess {
  unsigned long nextTickMillis;
  int tickInterval;
  Process* process;
};


class ProcessScheduler {
  std::vector<ScheduledProcess*> processes;

  public:
  /**
   * @brief Add a process to the scheduler
   *
   * @param process The process to add
   * @param tickInterval The interval in milliseconds to update the process
   */
  void addProcess(Process* process, int tickInterval) {
    processes.push_back(new ScheduledProcess{ millis(), tickInterval, process });
  }

  /**
   * @brief Update all processes
   * Needs to be called in the main loop as often as possible.
   */
  void update() {
    for (ScheduledProcess* process : processes) {
      long currentMillis = millis();
      if (currentMillis < process->nextTickMillis) continue;

      process->nextTickMillis = currentMillis + process->tickInterval; // update next tick
      long t = millis();
      process->process->update(); // Update the process

      long diff = millis() - t;
      bool processTookTooLong = process->tickInterval < diff;
      if (processTookTooLong) {
        printf("\033[1;31m%s took %dms\033[0m\n", process->process->getName().c_str(), diff);
      }
      debug("t%d\n", diff);
    }
  }
};