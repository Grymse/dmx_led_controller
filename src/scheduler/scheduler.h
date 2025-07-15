#pragma once

#include <Arduino.h>
#include <vector>
#include "debug.h"

/**
 * @brief An interface for a process that is updated at a regular interval.
 *
 */
class Process {
  public:
  /**
   * @brief Update the process.
   *
   */
  virtual void update() = 0;

  /**
   * @brief Get the Name process
   *
   */
  virtual String getName() = 0;
};

/**
 * @brief A struct to represent a scheduled process
 *
 */
struct ScheduledProcess {
  unsigned long nextTickMillis;
  int tickInterval;
  Process* process;
};


typedef void (*FutureFunction)();

struct TimeoutFunctions {
  FutureFunction func;
  unsigned long timeout;
};

class ProcessScheduler {
  std::vector<ScheduledProcess*> processes;
  std::vector<TimeoutFunctions*> timeouts;

  public:
  void addProcess(Process* process, int tickInterval);
  void update();
  void timeout(FutureFunction func, unsigned long millisDelay);
};