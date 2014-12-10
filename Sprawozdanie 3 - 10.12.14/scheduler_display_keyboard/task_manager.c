/*
 * task_manager.c
 *
 * Created: 2014-11-11 18:00:13
 * Authors: Damian Lesniak, Igor Sieradzki, Patrycja Cielecka
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "task_manager.h"

typedef struct {
	void (*foo)(void*);		//function intended to be executed
	void* args;				//arguments of function foo
	uint8_t interval;		//interval of continuous executions of function
	uint16_t counter;		//counter of executions
} task;

int tasksToDo = 0;

static task tasks[MAX_NUMBER_OF_TASKS];

void schedule(){
	
	for (int i=0; i<MAX_NUMBER_OF_TASKS; i++){					// checking and incrementing loop
		tasks[i].counter++;											// time passes, increase every task's counter
		if (tasks[i].counter % tasks[i].interval == 0) tasksToDo++;	// check if one of the functions need to be executed now
	}	
	
}

void addTask(uint8_t numberOfTask, uint8_t interval, void (*foo)(void*), void* args){
	
	tasks[numberOfTask].foo = foo;
	tasks[numberOfTask].args = args;
	tasks[numberOfTask].interval = interval;
	tasks[numberOfTask].counter = 0;
	
}

void execute(){
		
	while (1){
	
		
		while ( tasksToDo == 0 ){}	  // empty loop for idling (when ther;'s no task to do) 
		
		int8_t currentTaskToDo = -1;  // for keeping watch on which task to execute after the checking loop
		cli();						  // turn interrupts off, for synchronicity's sake 
		
		for (int i=0; i<MAX_NUMBER_OF_TASKS; i++)								// checking loop
		{									
			if ( tasks[i].interval > 0 && tasks[i].counter >= tasks[i].interval )	// check if there's a task to execute
			{																		// if there is...
				tasks[i].counter -= tasks[i].interval;									// decrease its counter by its interval (1 execution) 
				currentTaskToDo = i;														// remember which task it was
				tasksToDo--;															// decrease number of task that still need executing 
				break;																	// finally, leave the loop for imminent execution
			}
		}
		
		sei();												// turn interrupts on again
		tasks[currentTaskToDo].foo(tasks[currentTaskToDo].args);	// execute the remembered task
		
	}
	
}