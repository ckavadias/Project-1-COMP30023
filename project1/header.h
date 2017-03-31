/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

/*Module containing function prototypes and structs used to describe processes 
and system variables during simulation*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#define DEBUG 1

/*Struct Definitions*/

typedef struct process process_t;

//queue definition for queue admin
typedef struct {
	process_t* start;
	process_t* end;
	int num_inqueue;
}queue_t;

//Struct definitions for system variables

typedef struct{
	int address;
	int size;
}memhole_t;

typedef struct {
	int numprocesses;
	int memusage;
	int quantum;
	int max_memory;
	int num_holes;
	int clock;
	memhole_t** memholes;
	int (*sort_list)(void*, void*);
}system_t;

//process structs
struct process{
	int arrival;
	int id;
	int memory_needed;
	int burst_left;
	int address;
	process_t* next_mem;
	process_t* next;
	process_t* next_disk;
	process_t* next_arrival;
};

/*Function prototypes*/
void debug(int number, char* message);
void debug_process(process_t* process);
void arrival_queue(queue_t* queue, FILE* openfile);
void initialise_system(system_t* system, queue_t* queue,int argc,char* argv[]);
int worst_fit(void* parent, void* child);
int first_fit(void* parent, void* child);
int best_fit(void* parent, void* child);
int null_check(void* pointer, int terminate);
void initialise_memholes(system_t* system);
process_t* remove_mem(queue_t* queue, process_t* process);
process_t* load(queue_t* disk, system_t* system, queue_t* memory, queue_t* rr);
void add_rr(queue_t* queue, process_t* process);
void find_hole(system_t* system, process_t* process, queue_t* memory,
										   queue_t* disk, queue_t* round_robin);
void swap(void** memholes, int first, int second);
void sift(void** memholes, int start, int end, 
							int (*sort_list)(void*, void*));
void heapify(int size, void** memholes,
								   int (*sort_list)(void*, void*));
void heapsort_t(void** memholes, int size, 
								int (*sort_list)(void*, void*));
void add_hole(int address, int size, system_t* system);
void delete_hole(memhole_t** memholes, int index, int size);
void check_contiguity(system_t* system);
void add_disk(process_t* process, queue_t* disk);
int memusage(queue_t* memory, system_t* system);
int p_sort(void* process1, void* process2);
process_t** p_array(process_t* process, process_t** array, int index, int* max);
void remove_rr(queue_t* queue, process_t* process);