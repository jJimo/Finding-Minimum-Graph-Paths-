#ifndef sccD
#define sccD

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include "bfs.h" 
#define OK_SUCCESS 1
#define FAILURE 0
#define K 10000000		//10 miria na goustaroume na 'oume

typedef int bool;
#define true 1
#define false 0

typedef struct {
	uint32_t start;
	uint32_t end;
}edge;


typedef struct NodeCC{
	uint32_t id;
	int index;
	int lowlink;
	uint32_t caller;
	int last_buffer_block;
	int last_buffer_entry;
	struct NodeCC* next;		//next element in stack
}NodeCC;

typedef struct StackCC{
	int size;
	NodeCC* start;
}StackCC;


typedef struct Component {
	uint32_t component_id; 																			//current component id
	uint32_t included_nodes_count; 																	//number of nodes in component
	uint32_t* included_node_ids;																	//ids of included nodes	
	uint32_t min_rank;
	uint32_t rank;
	uint32_t min_rank2;
	uint32_t rank2;
	uint32_t counter;
	uint32_t to_check;
	struct Component **c_neighbors;
	uint32_t compon_neig_size;
	struct Component *next;				
}Component;

typedef struct StronglyCC{
	Component *components; 																			//Components index - a vector which stores the components information
	uint32_t components_count;
	uint32_t *id_belongs_to_component; 																//inverted index
}SCC;

typedef struct CCursor{
	Component* component_ptr; 																		//pointer to current’s iteration component
}ComponentCursor;

SCC* CreateSCC(uint32_t);																			//creates a SCC struct and initiliazes its parameters
int InsertComponent(Component**, uint32_t, uint32_t, uint32_t*);									//Imports a new component at the list of components of SCC
void PrintAllComponents(Component *);
void freeComponents(Component** );
SCC* estimateStronglyConnectedComponents(NodeIndex *outcoming_index,MetaDataBuffer *b_out,uint32_t megethos);	//Computes SCCs
int findNodeStronglyConnectedComponentID(SCC *, uint32_t);											//Rerurns SCC which given node belongs to
bool iterateStronglyConnectedComponentID(SCC *, ComponentCursor **);								//Initializes componentCursor to first list component
bool next_StronglyConnectedComponentID(ComponentCursor *);											//Moves cursor to next component
int estimateShortestPathStronglyConnectedComponents();												//New bfs
void destroyStronglyConnectedComponents(SCC** );													//Destroys SCC

//bbfs for search in same SCC
int  examine_reachable_nodes_SCC(uint32_t *id_inverted,uint32_t target_Component,char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb);
int bfs_SCC(SCC* scc,NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors);


StackCC* StackCC_Creation();
int StackCC_Destruction(StackCC *S);
NodeCC* StackCC_Top(StackCC *S );
int StackCC_Push(StackCC *S , NodeCC node);
int StackCC_Pop(StackCC *S);
void StackCC_Print(StackCC *S);
void StackCC_PrintNode(NodeCC* n);


//Tarjan etc...
uint32_t bring_me_next_neighbor(NodeCC* n,NodeIndex *index,MetaDataBuffer *mbuffer);
void tarjan_iter(uint32_t node,NodeCC *nodes,uint32_t *index,char *onStack,StackCC *S,char *defined_nodes,NodeIndex *indexMeta,MetaDataBuffer *mbuffer,SCC **scc);
void Tarjan(NodeIndex *indexMeta,MetaDataBuffer *mbuffer, SCC **scc, uint32_t size);


#endif
