/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

#include "header.h"


int
main(int argc, char *argv[]){
	system_t system;
	queue_t arrivals, round_robin, memory, disk;
	process_t* current, **priority;
	int run_time = 0, address = -1, max = 1, num_arrived, i;
	
	//erase previous debugging record
	dopen("debug.txt", "w");
	dclose(debug_f);
	
	initialise_system(&system, &arrivals, argc, argv);
	initialise_queue(&memory);
	initialise_queue(&disk);
	initialise_queue(&round_robin);
	
	debug(0, "initialisation complete");
	
	//while loop simulates clock ticks in increments of 1
	while(arrivals.num_inqueue || round_robin.num_inqueue){
		
		//E3 eventuality
	  if(round_robin.num_inqueue && round_robin.start->burst_left == 0){
	  	  	debug(system.clock, "\nEventuality E3");
	  	  	
	  	  	print_holes(system.memholes, system.num_holes);
	  	  	print_rr(&round_robin);
	  	  	
	  	  	current = round_robin.start;
	  	  	remove_mem(&memory, round_robin.start);
	  	  	add_hole(round_robin.start->address, 
	  	  		round_robin.start->memory_needed,&system);
	  	  	
			remove_rr(&round_robin, round_robin.start);
			add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
			debug(0, "Process added to rr");
			free(current);
			run_time = 0;
		}
		
		//E2 eventuality
		if(run_time == system.quantum){
			debug(system.clock, "\nEventuality E2");
			print_holes(system.memholes, system.num_holes);
			print_rr(&round_robin);
			current = round_robin.start;
			add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
		
			//if still in memory put current at end of queue
			if(null_check(round_robin.start, 0)){
					if(current->id == round_robin.start->id){
						address = current->address;
						remove_rr(&round_robin, current);
						current->address = address;
						add_rr(&round_robin, current);
						current = NULL;
					}
			}
			run_time = 0;
		}
		
		debug(0, "Entering E1");
		//E1 eventuality
		if(arrivals.num_inqueue && system.clock == arrivals.start->arrival){
			debug(system.clock, "\nEventuality E1");
			print_holes(system.memholes, system.num_holes);
			print_rr(&round_robin);
			
			num_arrived = 0;
			max = arrivals.num_inqueue;
			priority = (process_t**)malloc(sizeof(process_t*)*max);
			
			//record all arrivals, sort by priority then add to disk
			while(arrivals.start && system.clock == arrivals.start->arrival){
				debug(0, "E1 first loop");
				
				current = arrivals.start;
				arrivals.start = current->next_arrival;
				current->next_arrival = NULL;
				
				priority = p_array(current, priority, num_arrived, &max);
				num_arrived++;
				
				arrivals.num_inqueue--;
			}
			
			heapsort_t(priority, num_arrived, p_sort);
			for(i = 0; i < num_arrived; i++){
					add_disk(priority[i], &disk);
			}
				
			if(!memory.num_inqueue){
			  debug(0, "Adding arrival to queue");
			  add_rr(&round_robin, load(&disk, &system, &memory, &round_robin));
			}
			
		}
		
		if(null_check(round_robin.start, 0)){
				round_robin.start->burst_left--;
		}
		
		run_time++;
		system.clock++;
	}
	debug(0, "Exit clock incrementing");
	//print end ouput line may need to lower clock value
	
	system.clock--;
	printf("time %d, simulation finished.\n", system.clock);
	return 0;
}