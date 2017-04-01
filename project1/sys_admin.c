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
		dopen("debug.txt", "a");
		fprintf(debug_f, "%s %d\n", message, number);
		dclose(debug_f);
	}
}

//open and close debugging record file
void dopen(char* name, char* mode){
	debug_f = fopen(name, mode);
}
void dclose(FILE* file){
	fclose(file);
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

//function to create queue of processes
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
		current->next_arrival = (process_t*)malloc(sizeof(process_t));
		null_check(current->next_arrival, 1);
		queue->num_inqueue++;
		
		queue->end = current;
		current = current->next_arrival;
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
					if(strcmp(optarg, "best") == 0){
							debug(0, "best_fit assigned");
							system->sort_list = (*best_fit);
					}
					else if(strcmp(optarg, "first") == 0){
							debug(0, "first_fit assigned");
							system->sort_list = (*first_fit);
					}
					else if(strcmp(optarg, "worst") == 0){
							debug(0, "best_fit assigned");
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
	debug(0, "start initialising memholes array");
  system->memholes = (memhole_t**)malloc(sizeof(memhole_t*)*system->max_memory);
	null_check(system->memholes, 1);
	system->num_holes = 1;
	system->clock = 0;
	(system->memholes)[0] = (memhole_t*)malloc(sizeof(memhole_t));
	null_check((system->memholes)[0], 1);
	(system->memholes[0])->address = system->max_memory;
	(system->memholes[0])->size = system->max_memory;
	debug(0, "__initialise_system finished");
	
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
	
	debug(0, "__remove_mem start");
	current = queue->start;
	previous = NULL;
	
	while(current){
		
		if(current->id == process->id){
			
			//check if start of queue
			if(null_check(previous, 0)){
				debug(0, "		inside queue");
				previous->next_mem = current->next_mem;
			}
			else{
				debug(0, "		start of queue");
				debug(null_check(current, 0), "		void status of current");
				queue->start = current->next_mem;
				debug(0, "		current removed");
				
			}
			if (!null_check(current->next_mem, 0)){
				queue->end = previous;
				
				if(null_check(previous, 0)){
						previous->next = NULL;
				}
			}
			current->next_mem = NULL;
			break;
		}
		
		previous = current;
		current = current->next_mem;
	}
	queue->num_inqueue--;
	debug(0, "__remove_mem end");
	return process;
}

//load a process from disk if one is there
process_t* load(queue_t* disk, system_t* system, queue_t* memory, queue_t* rr){
	process_t* process;
	
	debug(0, "__load start");
	process = disk->start;
	
	if(null_check(process, 0)){
		debug(0, "		process not NULL");
		
		//find hole
		find_hole(system, process, memory, disk, rr);
		
		//add process to memory
		if(!memory->num_inqueue){
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
		
		if(disk->start == disk->end){
			disk->start = NULL;
			disk->end = NULL;
		}
		else{
			disk->start = disk->start->next_disk;
			process->next_disk = NULL;
		}
		disk->num_inqueue--;
		memory->num_inqueue++;
		
	printf("time %d, %d loaded, numprocesses=%d, numholes=%d, memusage=%d%%\n",
		system->clock, process->id, memory->num_inqueue, system->num_holes,
		memusage(memory, system));
	}
	debug(0, "__load end");
	return process;
}

//add process loaded from disk to round robin queue
void add_rr(queue_t* queue, process_t* process){
	
	debug(0, "__add_rr start");
	//process is void
	if(!null_check(process, 0)){
		return;
	}
	
	//process not void
	else{
		debug(0, "process not void");
		//queue is empty
		if(queue->start == NULL && queue->end == NULL){
			debug(0, "		empty rr queue");
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
			debug(0, "		rr queue not empty");
			queue->end->next = process;
			queue->end = process;
			process->next = NULL;
		}
		
		queue->num_inqueue++;
	}
	debug(0, "__add_rr finish");
}
//find hole to place process in memory or create if necesary
void find_hole(system_t* system, process_t* process, queue_t* memory,
										   queue_t* disk, queue_t* round_robin){
	process_t* removed_p = NULL, **removed;
	int found = 0, i, address, size, num_disk = 0, max = 10;
	
	debug(0, "__find_hole start");
	removed = (process_t**)malloc(sizeof(process_t*)*max);
	null_check(removed, 1);
	
	
	while(!found){
		
		for(i = 0; i < system->num_holes; i++){
			debug(system->num_holes, "find_hole inner for loop active");
			if(process->memory_needed <= (system->memholes[i])->size){
				debug(0,"		hole found");
				process->address = (system->memholes[i])->address;
				
				debug(process->address, "		process address: ");
				
				if(process->memory_needed < (system->memholes[i])->size){
					debug(0, "		hole more than needed");
					size = (system->memholes[i])->size - process->memory_needed;
					address = (system->memholes[i])->address - 
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
				break;
			}
		}
		
		if(found){
			break;
		}
		
		else{
			debug(0, "no hole found");
			removed_p = remove_mem(memory, memory->start);
			debug(num_disk,"removed_p assigned");
			//debug(removed_p->address, "remove_p address: ");
			null_check(removed_p, 1);
			add_hole(removed_p->address, removed_p->memory_needed, system);
			remove_rr(round_robin, removed_p);
		
			removed = p_array(removed_p, removed, num_disk, &max);
			num_disk++;
			}
		
	}
	
	if(num_disk){
		
		heapsort_t(removed, num_disk, p_sort);
	
		for(i = 0; i < num_disk; i++){
			add_disk(removed[i], disk);
		}
	}
	
	free(removed);
	debug(0, "__find_hole end");
}

//add hole at location address and of size size
void add_hole(int address, int size, system_t* system){
	int num = system->num_holes;
	
	debug(0, "__add_hole start");
	(system->memholes)[num] = (memhole_t*)malloc(sizeof(memhole_t));
	null_check((system->memholes)[num], 1);
	system->num_holes++;
	
	(system->memholes)[num]->address = address;
	debug(address, "		hole address");
	debug(size, "		hole size");
	(system->memholes)[num]->size = size;
	
	heapsort_t(system->memholes, system->num_holes, first_fit);
	
	//check for contiguity
	print_holes(system->memholes, system->num_holes);
	check_contiguity(system);
	print_holes(system->memholes, system->num_holes);
	
	//sort list back into desired order
	heapsort_t(system->memholes, system->num_holes, system->sort_list);
	debug(0,"__add_hole end");
}

//check for contiguous holes and combine
void check_contiguity(system_t* system){
	int i = 0;
	
	debug(0, "__check_contiguity start");
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
	debug(0, "__check_contiguity end");
}

//delete array entry in memholes
void delete_hole(memhole_t** memholes, int index, int size){
	int i;
	
	debug(size, "__delete_hole start, size is");
	for(i = index; i < size -1; i++){
		memholes[i] = memholes[i + 1];
	}
	
	memholes[i] = NULL;
	debug(0, "__delete_hole finish");
}

//add a process to the end of the disk queue
void add_disk(process_t* process, queue_t* disk){
	
	debug(0, "__add_disk start");
	if(!null_check(disk->start, 0) && !null_check(disk->end, 0)){
		debug(0, "NULL disk");
		disk->start = process;
		disk->end = process;
		debug(0, "end/start assigned");
		debug(process->id, "	Current process is: ");
		process->next_disk = NULL;
		debug(0, "next_disk assigned");
		
	}
	
	else{
		debug(0, "		add to end of queue");
		disk->end->next_disk = process;
		disk->end = process;
		debug(null_check(process, 0), "		process null status");
		process->next_disk = NULL;
	}
	debug(0, "__add_disk end");
}

//remove given process from round robin chronological listing
void remove_rr(queue_t* queue, process_t* process){
	process_t* current, *previous;
	
	current = queue->start;
	previous = NULL;
	
	debug(0, "__remove_rr starts");
	while(current){
		
		if(current->id == process->id){
			debug(current->id, "		process id ");
			//check if start of queue
			if(null_check(previous, 0)){
				previous->next = current->next;
			}
			else{
				queue->start = current->next;
				
			}
			if (!null_check(current->next, 0)){
				queue->end = previous;
				if(null_check(previous, 0)){
						previous->next = NULL;
				}
			}
			current->address = -1;
			current->next = NULL;
			break;
		}
		
		previous = current;
		current = current->next;
	}
	
	debug(0, "__remove_rr ends");
	queue->num_inqueue--;
}

//calculate memusage
int memusage(queue_t* memory, system_t* system){
	process_t* current = memory->start;
	double usage = 0;
	int percentage;
	
	while(current){
		usage += current->memory_needed;
		current = current->next_mem;
	}
	
	percentage = (int)(100*usage)/system->max_memory;
	
	if((100*usage)/system->max_memory - (double)percentage > 0.00){
		percentage++;
	}
	
	return percentage;
	
	
}

//adds process ready to go on disk to array for sorting
process_t** p_array(process_t* process, process_t** array, int index, int* max){
	
	debug(0, "__p_array starts");
	if( index >= *max){
		(*max)*=2;
		array = (process_t**)realloc(array, sizeof(process_t*)*(*max));
	}
	
	array[index] = process;
	
	debug(0, "__p_array ends");
	return array;
}

//set all queue values to NULL and 0
void initialise_queue(queue_t* queue){
	queue->start = NULL;
	queue->end = NULL;
	queue->num_inqueue = 0;
}

//print all memory holes
void print_holes(memhole_t** memholes, int size){
	int i;
	
	if(DEBUG){
		printf("\n\n");
		for(i = 0; i < size; i++){
			printf("Hole Address: %d, Hole Size: %d\n", memholes[i]->address,
				memholes[i]->size);
		}
	}
}

//print round robine queue
void print_rr(queue_t* queue){
	process_t* current = queue->start;
	
	if(DEBUG){
		printf("\n\n");
		while(current){
			printf("Process Address: %d, Process Size: %d, Process ID: %d\n", 
				current->address,current->memory_needed, current->id);
			current = current->next;
		}
	}
}