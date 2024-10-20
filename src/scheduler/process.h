#ifndef PROCESS_H
#define PROCESS_H

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

#endif
