/*Project 1 COMP30023, University of Melbourne, Semester 1 2017
Author: Constantinos Kavadias, Student ID: 664790, user_id: ckavadias
ckavadias@unimelb.edu.au, March/April 2017*/

/*module containing functions necessary to facilitate chosen memory fit
algorithm choosing between best, first and worst fit*/

#include "header.h"

/*Function Definitions*/

//Best fit list sort function (ascending size)
int best_fit(void* parent, void* child){
	
	if(((memhole_t*)parent)->size < ((memhole_t*)child)->size){
		return 1;
	}
	
	return 0;
}

//First fit list sort function (descending address)
int first_fit(void* parent, void* child){
	
	if(((memhole_t*)parent)->address > ((memhole_t*)child)->address){
		return 1;
	}
	
	return 0;
}

//Worst fit list sort function (descending size)
int worst_fit(void* parent, void* child){
	
	if(((memhole_t*)parent)->size > ((memhole_t*)child)->size){
		return 1;
	}
	
	return 0;
	
}

//heapsort algorithm that takes a function argument to determine sort priority
//guidance from https://en.wikipedia.org/wiki/Heapsort 
//and https://tinyurl.com/keq7e4q
void heapsort_t(void** memholes, int size, 
								int (*sort_list)(void*, void*)){
	debug(0, "__heapsort start");
	//heapify the array
	heapify(size, memholes, sort_list);
	
	while(size > 0){
		swap(memholes, size - 1, 0);
		size--;
		sift(memholes, 0, size -1, sort_list);
	}
	debug(0, "__heapsort end");
}

//restore heap property to an array
void heapify(int size, void** memholes,
								   int (*sort_list)(void*, void*)){
	int start = (size-1)/2;
	
	debug(0, "__heapify start");
	while(start >= 0){
		sift(memholes, start, size -1, sort_list);
		start--;
	}
	debug(0, "__heapify end");
}

//sifting function to assist with heapifying
void sift(void** memholes, int start, int end, 
							int (*sort_list)(void*, void*)){
	int child, root;
	
	debug(0, "		__sift start");
	while(2*start + 1 <= end){
		child = 2*start + 1;
		root = start;
		
		if(sort_list(memholes[root], memholes[child])){
			root = child;
		}
		if(child + 1 <= end && sort_list(memholes[root], memholes[child +1])){
			root = child + 1;
		}
		if(root == start){
			debug(0, "		__sift end at start");
			return;
		}
		
		swap(memholes, start, root);
		start = root;
	}
	debug(0, "		__sift end");
	
}

//swap two array objects at given indices
void swap(void** memholes, int first, int second){
	memhole_t* temporary;
	
	debug(0, "		__swap start");
	temporary = memholes[first];
	memholes[first] = memholes[second];
	memholes[second] = temporary;
	debug(0, "		__swap end");
	return;
}

//sort processes based on priority
int p_sort(void* process1, void* process2){
	
	if(((process_t*)process1)->id < ((process_t*)process2)->id){
		return 1;
	}
	
	return 0;
}



