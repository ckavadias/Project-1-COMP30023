/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

#include "header.h"


int
main(int argc, char *argv[]){
	system_t system;
	queue_t arrivals, round_robin, memory, disk;
	process_t* current, *previous, *highest, *highest_p;
	int run_time = 0, new_process = 1, clock = 0, priority = 0;
	
	initialise_system(&system, &arrivals, argc, argv);
	memory.num_inqueue = 0;
	disk.num_inqueue = 0;
	round_robin.num_inqueue = 0;
	exit(EXIT_SUCCESS);
	
	//while loop simulates clock ticks in increments of 1
	while(arrivals.num_inqueue || round_robin.num_inqueue){
		
		//E3 eventuality
	  if(null_check(round_robin.start,0) && round_robin.start->burst_left == 0){
			//delete process from memory
			//move queue up
			//swap and schedule
		}
		
		//E2 eventuality
		if(run_time == system.quantum){
			//swap and schedule
			//put to end of queue
			//should come after potential swapped in process
		}
		
		//E1 eventuality
		if(clock == arrivals.start->arrival){
			priority = 0;
			current = arrivals.start;
			previous = NULL;
			
			while(clock == current->arrival){
				
				if(current->id >= priority){
					priority = current->id;
					highest = current;
					highest_p = previous;
				}
				previous = current;
				current = current->next;
			}
			
			//not at start of queue
			if(highest_p){
				highest_p->next = highest->next;
			}
			//at end of queue
			else if(!null_check(highest->next,0)){
				arrivals.end = highest_p;
				highest_p = NULL;
			}
			//otherwise at start of queue
			else{
				arrivals.start = highest->next;
			}
			
			//add to disk
			
			if(!memory.num_inqueue){
				//swap and schedule
			}
			
		}
		
		
		arrivals.start->burst_left--;
		run_time++;
		clock++;
	}
	
	//print end ouput line may need to lower clock value
	
	return 0;
}