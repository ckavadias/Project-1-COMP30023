/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

#include "header.h"


int
main(int argc, char *argv[]){
	system_t system;
	queue_t arrivals, round_robin, memory, disk;
	process_t* current, *previous, *highest, *highest_p;
	int run_time = 0, priority = 0;
	
	initialise_system(&system, &arrivals, argc, argv);
	memory.num_inqueue = 0;
	disk.num_inqueue = 0;
	round_robin.num_inqueue = 0;
	exit(EXIT_SUCCESS);
	
	//while loop simulates clock ticks in increments of 1
	while(arrivals.num_inqueue || round_robin.num_inqueue){
		
		//E3 eventuality
	  if(round_robin.num_inqueue && round_robin.start->burst_left == 0){
	  	  	remove_mem(&memory, round_robin.start);
			remove_rr(&round_robin, round_robin.start);
			add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
		}
		
		//E2 eventuality
		if(run_time == system.quantum){
			
			current = round_robin.start;
			add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
		
			//if still in memory put current at end of queue
			if(null_check(round_robin.start, 0)){
					if(current->id == round_robin.start->id){
						remove_rr(&round_robin, current);
						add_rr(&round_robin, current);
						current = NULL;
					}
			}
		}
		
		//E1 eventuality
		if(system.clock == arrivals.start->arrival){
			
			//nested loop necessary as all arriving processes must be added
			//to disk in correct priority order
			while(system.clock == current->arrival){
				
				priority = 0;
				current = arrivals.start;
				previous = NULL;
				highest = NULL;
				highest_p = NULL;
				
				while(system.clock == current->arrival){
					
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
				if(!null_check(highest->next,0)){
					arrivals.end = highest_p;
					highest_p = NULL;
				}
				//otherwise at start of queue
				else{
					arrivals.start = highest->next;
				}
				
				//add to disk
				add_disk(highest_p, &disk);
				arrivals.num_inqueue--;
			}
			
			if(!memory.num_inqueue){
			  add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
			}
			
		}
		
		
		arrivals.start->burst_left--;
		run_time++;
		system.clock++;
	}
	
	//print end ouput line may need to lower clock value
	
	return 0;
}