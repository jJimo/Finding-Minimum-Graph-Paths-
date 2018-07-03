#ifndef list
#define list

#include "index.h"

#define SUCCESS 1
#define FAILURE 0
typedef struct reachable_node * reachable_node_pointer;

//In this file are included the prototypes of the function for handling the returning lists at every BFS'S step

//List_header
typedef struct{
	int size;	// the size of the list(i.g. the number of its nodes)
	int step;	// the step represents the step of BFS search,the number of the step that supposed list of reachable nodes has been discovered
	reachable_node_pointer start;		//pointer to the first node of the list
}List;


List* creation(int step);			
int  destruction(List* l);
int add_node(List* l,uint32_t n);		//function for insertion of the new node  ONLY TO THE START of the list
void print_list(List* l);				
//int exists_in_list(List* L,uint32_t n);
int is_empty(List* l);
 
 
#endif
