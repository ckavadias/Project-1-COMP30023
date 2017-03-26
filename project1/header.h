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

//queue definition for queue admin
typedef struct {
	void* start;
	void* end;
	int num_inqueue;
}queue_t;

//Struct definitions for system variables
typedef struct memhole memhole_t;

struct memhole{
	int address;
	int size;
	memhole_t* next;
	memhole_t* previous;
};

typedef struct {
	queue_t* memholes;
	int numprocesses;
	int memusage;
	int quantum;
	int max_memory;
	void (*add_list)(queue_t*, memhole_t*);
}system_t;

//process structs
typedef struct process process_t;

struct process{
	int next_arrival;
	int id;
	int memory_needed;
	int burst_left;
	int loaded;
	process_t* next;
	process_t* prev;
};

/*Function prototypes*/
void debug(int number, char* message);
void debug_process(process_t* process);
void process_queue(queue_t* queue, FILE* openfile);
void initialise_system(system_t* system, queue_t* queue,int argc,char* argv[]);
void worst_fit(queue_t* queue, memhole_t* memhole);
void first_fit(queue_t* queue, memhole_t* memhole);
void best_fit(queue_t* queue, memhole_t* memhole);
