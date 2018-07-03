#ifndef grailD
#define grailD

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include "StronglyCC.h"
#define OK_SUCCESS 1
#define FAILURE 0

typedef int GRAIL_ANSWER;
enum GRAIL_ANSWER{NO=0, MAYBE=1, YES=2};



typedef struct NodePOT{
	Component *comp;		//pointer to the component in order to not spend time copy-paste them
	struct NodePOT* next;		//next element in stack
}NodePOT;

typedef struct StackPOT{
	int size;
	NodePOT* start;
}StackPOT;


void constructHyperGraph(SCC **scc, NodeIndex *index, MetaDataBuffer *buffer);
void buildGrailIndex(SCC *scc);
int createRanks(SCC *scc,Component **hash, char *byte_array,  uint32_t start);
int createRanks2(SCC *scc,Component **hash, char *byte_array,  uint32_t start) ;
Component **createComponentsArray(SCC *);		//creates hash 
Component * bring_me_the_neighbor(SCC **scc,uint32_t number);

GRAIL_ANSWER isReachableGrailIndex(SCC *scc, Component **hash, uint32_t source_node,uint32_t target_node);

// -------------------------- BFS USING GRAIL
int  examine_reachable_nodes_GRAIL(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb,SCC *scc,Component **hash,uint32_t node);
int bfs_GRAIL(NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors,SCC *scc,Component **hash);

//Functionalities for the POST ORDER TRAVERSAL STACK
StackPOT* StackPOT_Creation();
int StackPOT_Destruction(StackPOT *S);
NodePOT* StackPOT_Top(StackPOT *S );
int StackPOT_Push(StackPOT *S , Component *node);
int StackPOT_Pop(StackPOT *S);
void StackPOT_Print(StackPOT *S);


#endif
