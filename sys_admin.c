/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

/*Module containing function definitions used to describe processes and 
system variables during simulation*/

#include "header.h"

/*external variables*/
extern char* optarg;


/*Function Definitions*/

//debugging integers
void debug(int number, char* message){
	if(DEBUG){
		fprintf(stderr, "%s %d\n", message, number);
	}
}

//print out contents of process for debugging
void debug_process(process_t* process){
	if(DEBUG){
		fprintf(stderr, "	Arrival: %d\n", process->arrival);
		fprintf(stderr, "	id: %d\n", process->id);
		fprintf(stderr, "	Memory: %d\n", process->memory_needed);
		fprintf(stderr, "	Burst time: %d\n", process->burst_left);
	}
}

//function to create queue of process
void process_queue(queue_t* queue, FILE* openfile){
	process_t* current = NULL, *previous = NULL;
	
	queue->num_inqueue = 0;
	queue->start = (process_t*)malloc(sizeof(process_t));
	null_check(queue->start, 1);
	
	current = queue->start;
	//scan file line by line (process by process) and record in queue 
	while(!feof(openfile)){
		
		fscanf(openfile, "%d %d %d %d\n", &(current->arrival),
		&(current->id), &(current->memory_needed), &(current->burst_left));
	
		current->next = (process_t*)malloc(sizeof(process_t));
		null_check(current->next, 1);
		current->address = -1;
		queue->num_inqueue++;
		
		queue->end = current;
		current = current->next;
		debug(0, "still looping");
	}
	
	debug(queue->num_inqueue, "Number of processes: ");
	free(current);
	
}

//function to initialise system from command prompts including process queue
//getopt usability quoted from provided materials
void initialise_system(system_t* system, queue_t* queue,int argc,char* argv[]){
	char input, *filename;
	FILE* openfile = NULL;
	
	//parse command line arguments
	debug(0, "begin parsing");
	while((input = getopt(argc, argv, "f:m:a:q:")) != EOF){
	
			switch (input){
				case 'f':
					filename = optarg;
					debug(0, filename);
					break;
					
				case 'q':
					system->quantum = atoi(optarg);
					debug(system->quantum, "Quantum is: ");
					break;
					
				case 'm':
					system->max_memory = atoi(optarg);
					debug(system->max_memory, "Memory is: ");
					break;
					
				case 'a':
					if(strcmp(optarg, "best")){
							system->add_list = (*best_fit);
					}
					else if(strcmp(optarg, "first")){
							system->add_list = (*first_fit);
					}
					else if(strcmp(optarg, "worst")){
							system->add_list = (*worst_fit);
					}
					else{
						fprintf(stderr, "No appropriate algorithm selected");
						exit(EXIT_FAILURE);
					}
					debug(0, "Algorithm set");
					break;
				
				case '?':
					fprintf(stderr,"Options not declared correctly\n");
					fprintf(stderr, "Standard format is: ");
					fprintf(stderr, "-f filename -a (best, first or worst)");
				    fprintf(stderr, " -m memsize -q quantum\n");
				    fprintf(stderr, "Please try again\n");
				   break;
			}
	}
	debug(0, "end parsing");
	
	//read in filename and initialise the process queue
	openfile = fopen(filename, "r");
	process_queue(queue, openfile);
	fclose(openfile);
	
	//create memholes list, maximum number of holes is size of memory
	system->memholes = (memhole_t*)malloc(sizeof(memhole_t)*system->max_memory);
	null_check(system->memholes, 1);
	system->num_holes = 1;
	(system->memholes[0]).address = system->max_memory;
	(system->memholes[0]).size = system->max_memory;
	
	
}

//check if pointer is NULL if malloc check can activate termination clause
int null_check(void* pointer, int terminate){
	
	if(pointer == NULL){
		if(terminate){
			fprintf(stderr, "NULL pointer error");
		}
		return 0;
	}
	
	return 1;
}

//remove given process from memory chronological listing
void remove_mem(queue_t* queue, process_t* process){
	process_t* current, *previous;
	
	current = queue->start;
	
	while(current){
		
		if(current->id == process->id){
			if(null_check(previous, 0)){
				previous->next_mem = current->next_mem;
			}
			else{
				queue->start = current->next_mem;
				current->next = NULL;
				
			}
			if (!null_check(current->next_mem, 0)){
				queue->end = previous;
				previous->next = NULL;
			}
			break;
		}
		
		previous = current;
		current = current->next_mem;
	}
}

//check for contiguity after a process has been removed and update size of hole
void add_hole(process_t* process, system_t* system){
	memhole_t* current, *holder;
	int index;
	
	//add memory hole for removed process
	
	//bubble new entry into appropriate place in main list
	//sort memory hole list by address number in parallel
	//check for continguous chunks
	//check contiguity with lower address
	//perform appropriate deletion and redirection
	
	//check contiguity with higher address
	//perform appropriate deletion and redirection
}