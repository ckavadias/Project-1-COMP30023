/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790
ckavadias@unimelb.edu.au, March/April 2017*/

#include <stdio.h>
#include <stdlib.h>
#include "fit.c"
#include "sys_admin.c"


int
main(int argc, char *argv[]){
	system_t system;
	queue_t process_queue;
	int run_time = 0, new_process = 1, clock = 0;
	
	initialise_system(&system, &process_queue);
	
	//while loop simulates clock ticks in increments of 1
	while(process_queue.num_inqueue >= 0){
		
		if(run_time == system.quantum){
			//stop process, update start time and bubble through queue
			new_process = 1;
			run_time = 0;
		}
		
		if(new_process){
			//check if needs load to memory following necessary protocols
		}
		
		run_time++;
		clock++;
	}
	
	return 0;
}