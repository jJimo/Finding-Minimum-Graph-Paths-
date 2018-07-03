#ifndef indexD
#define indexD

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "buffer.h"
#define OK_SUCCESS 1
#define FAIL -1
#define DEFAULT_INDEX_SIZE 10


typedef struct Index_metadata{
	uint32_t size;				//size of index
	uint32_t *index;			//index with offset from buffer's first block which will include incoming/outcoming edges
}NodeIndex;



NodeIndex* createNodeIndex();
int increaseIndex(NodeIndex*, uint32_t);
int insertNode(NodeIndex *, MetaDataBuffer *, uint32_t , uint32_t, uint32_t );
int destroyNodeIndex(NodeIndex*);
void printIndex(NodeIndex *, MetaDataBuffer *);
ptr getListHead(NodeIndex*, uint32_t );

//============================Search function============================//
int search(NodeIndex *, MetaDataBuffer *, uint32_t , uint32_t);


#endif
