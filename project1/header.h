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
	memhole_t* memholes;
	void (*add_list)(queue_t*, memhole_t*);
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
void worst_fit(queue_t* queue, memhole_t* memhole);
void first_fit(queue_t* queue, memhole_t* memhole);
void best_fit(queue_t* queue, memhole_t* memhole);
int null_check(void* pointer, int terminate);
void initialise_memholes(system_t* system);
void remove_mem(queue_t* queue, process_t* process);
