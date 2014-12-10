/*
 * task_manager.h
 *
 * Created: 2014-11-11 17:27:14
 * Authors: Damian Lesniak, Igor Sieradzki, Patrycja Cielecka
 */ 


#ifndef TASK_MANAGER_H_
#define TASK_MANAGER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Maximum number of tasks.
 */
#define MAX_NUMBER_OF_TASKS 8

/**
 * Function increases counters in every task. It should be placed in interval interrupt handler code. When counter is bigger or equal to interval, task should be executed by execute function.
 */
void schedule();
/**
 * Function adds task to manager. 
 */
void addTask(uint8_t numberOfTask, uint8_t interval, void (*foo)(void*), void* args);
/**
 * This function contains never-ending loop which executes tasks intended to execute by function schedule. Function firstly checks tasks with the highest priority. If task counter (of executions of schedule function) is bigger than interval value, then task is executed. This function should be placed at the end of main function of aplication.
 */
void execute();

#endif /* TASK_MANAGER_H_ */