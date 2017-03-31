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
	process_t* current;
	
	queue->num_inqueue = 0;
	queue->start = (process_t*)malloc(sizeof(process_t));
	null_check(queue->start, 1);
	
	current = queue->start;
	//scan file line by line (process by process) and record in queue 
	while(!feof(openfile)){
		
		fscanf(openfile, "%d %d %d %d\n", &(current->arrival),
		&(current->id), &(current->memory_needed), &(current->burst_left));
		
		current->address = -1;
		current->next = (process_t*)malloc(sizeof(process_t));
		null_check(current->next, 1);
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
							system->sort_list = (*best_fit);
					}
					else if(strcmp(optarg, "first")){
							system->sort_list = (*first_fit);
					}
					else if(strcmp(optarg, "worst")){
							system->sort_list = (*worst_fit);
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
  system->memholes = (memhole_t**)malloc(sizeof(memhole_t*)*system->max_memory);
	null_check(system->memholes, 1);
	system->num_holes = 1;
	system->clock = 0;
	(system->memholes[0])->address = system->max_memory;
	(system->memholes[0])->size = system->max_memory;
	
	
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
process_t* remove_mem(queue_t* queue, process_t* process){
	process_t* current, *previous;
	
	current = queue->start;
	previous = NULL;
	
	while(current){
		
		if(current->id == process->id){
			
			//check if start of queue
			if(null_check(previous, 0)){
				previous->next_mem = current->next_mem;
			}
			else{
				queue->start = current->next_mem;
				
			}
			if (!null_check(current->next_mem, 0)){
				queue->end = previous;
				previous->next = NULL;
			}
			current->address = -1;
			current->next_mem = NULL;
			break;
		}
		
		previous = current;
		current = current->next_mem;
	}
	queue->num_inqueue--;
	return process;
}

//load a process from disk if one is there
process_t* load(queue_t* disk, system_t* system, queue_t* memory, queue_t* rr){
	process_t* process;
	
	process = disk->start;
	
	if(null_check(process, 0)){
		
		//find hole
		find_hole(system, process, memory, disk, rr);
		
		//add process to memory
		if(memory->start == NULL && memory->end == NULL){
			memory->start = process;
			memory->end = process;
			process->next_mem = NULL;
		}
		
		//provision for memory leak
		else if(memory->start == NULL && memory->end != NULL){
			fprintf(stderr, "Discontinuity in Memory queue, load");
			exit(EXIT_FAILURE);
		}
		
		//memory is not empty
		else{
			memory->end->next_mem = process;
			memory->end = process;
			process->next_mem = NULL;
		}
		disk->start = disk->start->next_disk;
		process->next_disk = NULL;
		memory->num_inqueue++;
		
	printf("time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%\n",
		system->clock, process->id, memory->num_inqueue, system->num_holes,
		memusage(memory, system));
	}
	
	return process;
}

//add process loaded from disk to round robin queue
void add_rr(queue_t* queue, process_t* process){
	//process is void
	if(!null_check(process, 0)){
		return;
	}
	
	//process not void
	else{
		//queue is empty
		if(queue->start == NULL && queue->end == NULL){
			queue->start = process;
			queue->end = process;
			process->next = NULL;
		}
		
		//provision for memory leak
		else if(queue->start == NULL && queue->end != NULL){
			fprintf(stderr, "Discontinuity in Round Robin, add_rr");
			exit(EXIT_FAILURE);
		}
		
		//queue is not empty
		else{
			queue->end->next = process;
			queue->end = process;
			process->next = NULL;
		}
		
		queue->num_inqueue++;
	}
}
//find hole to place process in memory or create if necesary
void find_hole(system_t* system, process_t* process, queue_t* memory,
										   queue_t* disk, queue_t* round_robin){
	process_t* removed_p = NULL, **removed;
	int found = 0, i, address, size, num_disk = 0, priority = 0, max = 10;
	
	
	removed = (process_t**)malloc(sizeof(process_t*)*max);
	null_check(removed, 1);
	
	while(!found){
		
		for(i = 0; i < system->num_holes; i++){
			if(process->memory_needed <= (system->memholes[i])->size){
				
				if(process->memory_needed < (system->memholes[i])->size){
					
					size = (system->memholes[i])->size - process->memory_needed;
					address = (system->memholes[i])->address + 
														process->memory_needed;
					delete_hole(system->memholes, i, system->num_holes);
					system->num_holes--;
					add_hole(address, size, system);
					
				}
				
				else{
					delete_hole(system->memholes, i, system->num_holes);
					system->num_holes--;
				}
				
				found = 1;
			}
		}
		
		if(found){
			break;
		}
		
		else{
			
			removed_p = remove_mem(memory, memory->start);
			
			add_hole(removed_p->address, removed_p->memory_needed, system);
			remove_rr(round_robin, removed_p);
		
			removed = p_array(removed_p, removed, num_disk, &max);
			num_disk++;
			}
		
	}
	
	heapsort_t(removed, num_disk++, p_sort);
	
	for(i = 0; i < num_disk; i++){
		add_disk(removed[i], disk);
	}
	
	free(removed);
}

//add hole at location address and of size size
void add_hole(int address, int size, system_t* system){
	int num = system->num_holes;
	
	(system->memholes)[num] = (memhole_t*)malloc(sizeof(memhole_t));
	null_check((system->memholes)[num], 1);
	system->num_holes++;
	
	(system->memholes)[num]->address = address;
	(system->memholes)[num]->size = size;
	
	num++;
	heapsort_t(system->memholes, num, first_fit);
	//check for contiguity
	check_contiguity(system);
	//sort list back into desired order
	heapsort_t(system->memholes, num, system->sort_list);
}

//check for contiguous holes and combine
void check_contiguity(system_t* system){
	int i = 0;
	
	while(i < system->num_holes - 1){
		if((system->memholes)[i]->address - (system->memholes)[i]->size ==
			(system->memholes)[i+1]->address){
			
			(system->memholes)[i]->size += (system->memholes)[i+1]->size;
			delete_hole(system->memholes, i + 1, system->num_holes);
			system->num_holes--;
		}
		else{
			i++;
		}
	}
}

//delete array entry in memholes
void delete_hole(memhole_t** memholes, int index, int size){
	int i;
	
	memholes[index] = NULL;
	index++;
	
	for(i = index; i < size - 1; i++){
		memholes[index - 1] = memholes[index];
	}
	
	memholes[index] = NULL;
	
}

//add a process to the end of the disk queue
void add_disk(process_t* process, queue_t* disk){
	
	if(!null_check(disk->start, 0) && !null_check(disk->end, 0)){
		disk->start = process;
		disk->end = process;
		process->next_disk = NULL;
	}
	
	else{
		disk->end->next_disk = process;
		disk->end = process;
		process->next_disk = NULL;
	}
}

//remove given process from round robin chronological listing
void remove_rr(queue_t* queue, process_t* process){
	process_t* current, *previous;
	
	current = queue->start;
	previous = NULL;
	
	while(current){
		
		if(current->id == process->id){
			
			//check if start of queue
			if(null_check(previous, 0)){
				previous->next = current->next;
			}
			else{
				queue->start = current->next;
				
			}
			if (!null_check(current->next, 0)){
				queue->end = previous;
				previous->next = NULL;
			}
			current->address = -1;
			current->next = NULL;
			break;
		}
		
		previous = current;
		current = current->next;
	}
	
	queue->num_inqueue--;
}

//calculate memusage
int memusage(queue_t* memory, system_t* system){
	process_t* current = memory->start;
	int usage = 0;
	
	while(current){
		usage += current->memory_needed;
	}
	
	return usage/system->max_memory;
	
	
}

//adds process ready to go on disk to array for sorting
process_t** p_array(process_t* process, process_t** array, int index, int* max){
	
	if( index >= *max){
		(*max)*=2;
		array = (process_t**)realloc(array, sizeof(process_t*)*(*max));
	}
	
	array[index] = process;
	
	return array;
}