//  connectedComponents.h

#ifndef Cc
#define Cc


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "index.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define INIT_SIZE_OF_UPDATEINDEX	50
#define INIT_SIZE_OF_ARRAYS 10
#define  METRIC_VAL_LIMIT 0.3

#define  OK_SUCCESS		1
#define  ALREADY_EXIST		2
#define EXIST 100
#define NOT_EXIST -100

#define  GENERAL_ERROR			-1
#define  CCINDEX_REALLOC_ERROR	-2
#define  OUT_OF_RANGE			-3
#define  FREE_ERROR				-4
#define  UPDATEINDEX_REALLOC_ERROR	-5
#define  ESTIMATE_CC_ERROR		-6
#define  UNINIT					-7
#define  CCINDEX_GET_ERROR	-8
#define  GENERAL_INSERT_ERROR	-9
#define  GENERAL_ERROR_2 -10
#define  UPDATE_UNITIALISE -11


typedef struct CCArray{
	int size;
	uint32_t * ccs;
}CCArray;

typedef struct ByteTable{
	uint32_t size;
	char * ccTable;
} ByteTable;

typedef struct UpdateIndex{
	int size_of_ccUpdate;		//The current size of update Index
	int numOfRecs;		//The number of records that update Index contains
	ByteTable ** ccUpdate;
}UpdateIndex;

typedef struct CC{ 
	int ccCounter;
	int size_of_ccindex;  //The current size of ccindex
	int ccindexNumOfRecs;	//The number of records that ccindex contains
	uint32_t * ccindex; //CCindex
	
	//---UpdateIndex's variables---//
	UpdateIndex * updateIndex;
	float metricVal;
	float all_qs;				//To calculate metricVal
	float q_that_used_ui; //To calculate metricVal
	
}CC;


CC* estimateConnectedComponents(NodeIndex *, NodeIndex *, MetaDataBuffer *, MetaDataBuffer *);

int findCC(CC *, NodeIndex *, NodeIndex *, MetaDataBuffer *, MetaDataBuffer *, char *, uint32_t, uint32_t); 

int insertIntoCCIndex(CC * cc, uint32_t nodeId, uint32_t ccNumId);

int insertNewEdgeToCC(CC * cc, uint32_t nodeIdS, uint32_t nodeIdE);
																		
uint32_t findNodeConnectedComponentID(CC * cc, uint32_t nodeId);

int rebuildIndexes(CC ** cc);				

CC * createConnectedComponents(uint32_t initSizeOfCCIndex);

int destroyConnectedComponents(CC * cc);
												
///--- UpdateIndex ---///

UpdateIndex * createUpdateIndex(int InitSizeOfUpdateIndex);

int destroyUpdateIndex(CC * cc);

int clearUpdateIndex(CC * cc);

int insertNewCCPairIntoUpdateIndex(CC * cc, uint32_t ccA, uint32_t ccB);

int findPairIntoUpdateIndex(CC * cc,uint32_t ccA,uint32_t ccB);

void mergeByteArrays(UpdateIndex * upIndex,uint32_t ccA,uint32_t ccB);

int reallocUpdateIndex(UpdateIndex * upIndex,uint32_t ccA);


///--------------------///

int searchForPath(CC * cc,uint32_t nodeA,uint32_t nodeB); 
//this function find if a path exist, return -1 if there is not exist

int metricReturn(CC * cc);

int checkMetricToRebuildIndexes(CC * cc);




#endif
