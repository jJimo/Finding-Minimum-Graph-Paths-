#include "list.h"

 
//Header for each node
typedef struct reachable_node{ 
	uint32_t node;			//info of the node
	reachable_node_pointer next;	//pointer to the next node
}reachable_node;


List* creation(int step){
	//that function mallocs and returns the header of the new list,initializes size to 0 and step to the number passed through the args
	List* l;
	l=malloc(sizeof(List));
	if (l==NULL)
		return NULL;
	l->size=0;
	l->step=step;
	l->start=NULL;
	return l;
}

int is_empty(List* l){
	//checks if the list is empty 
	return (l->size==0);
}

int destruction(List* l){
	//this function destroys the list ,list pointer given-through-the-args points to.
	//if the list is empty,function destroys only the header 
	reachable_node_pointer todel,todel2;
	if (!is_empty(l)){
		todel=l->start;
		while (todel!=NULL){
			todel2=todel;
			todel=todel->next;
			free(todel2);
		}
		free(l);
		l=NULL;
	}
	else{
		free(l);
		l=NULL;
	}
	return SUCCESS;
}

int  add_node(List* l,uint32_t n){
	//This function enters n at the start of the list pointed by L.
	//If list is empty then we make the pointer of the header node points to the newly-malloced/inserted node
	//In any case we increase the size of list
	reachable_node_pointer temp;
	temp=malloc(sizeof(reachable_node));
	if (temp==NULL)
		return FAILURE;
	temp->node=n;
	if (l->size==0){
		temp->next=NULL;
		l->start=temp;
	}
	else{
		temp->next=l->start;
		l->start=temp;
	}
	l->size++;
	return 1;
}



void print_list(List* l){
	//This assistive function prints the list if it is not empty or NULL
	//In any case special messages are printed
	reachable_node_pointer temp;
	if (l!=NULL){
		temp=l->start;
		if (temp==NULL)
			printf("H lista einai kenh\n");
		while (temp!=NULL){
			printf(" % "SCNd32" ->",temp->node);
			temp=temp->next;
		}
		printf("\n");
	}
	else
		printf("The to-be-printed list is NULL\n");
}



























