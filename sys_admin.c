/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790
ckavadias@unimelb.edu.au, March/April 2017*/

/*Module containing functions and structs used to describe processes and 
system variables during simulation*/


/*Struct Definitions*/

//queue definition for queue admin
typedef struct {
	void* start_queue;
	void* end_queue;
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
	void (*add_list)(memhole_t*, memhole_t*)
}system_t;

//process structs
typedef struct process process_t;

struct process{
	int next_arrival;
	int id;
	int memory_needed;
	int burst_left;
	process_t* next;
	process_t* prev;
};

/*Function Definitions*/

//function to create queue of process
void process_queue(queue_t* queue){
}

//function to initialise processes from input
void initialise_process(process_t* process){
}

//function to initialise system from command prompts including process queue
void initialise_system(system_t* system, queue_t* queue){
}
