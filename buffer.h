#ifndef buffD
#define buffD

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#define N 20
#define SIZE_OF_BUFFER 100

#define OK_SUCCESS 1
#define METABUFFERS_ERROR -1
#define BUFFERS_ERROR -2
#define REALLOCS_ERROR -3
#define EMPTY_BUFFERS_ERROR -4
#define PTR_OUT_OF_RANGE -5


typedef uint32_t ptr;

typedef struct list_node{
	uint32_t neighbor[N];
	uint32_t edgeProperty[N];
	ptr nextListNode;				
}list_node;


typedef struct MetaDataBuffer{	
	uint32_t size;	//size of buffer
	uint32_t insert_pos; //insert position. In a new adding we set the new value in this this position (node).
	list_node * buffer;		//This is the pointer of the buffer.
}MetaDataBuffer;


MetaDataBuffer * createBuffer(void);	

ptr allocNewNode(MetaDataBuffer **);

list_node * getListNode(MetaDataBuffer *,ptr);

int destroyBuffer(MetaDataBuffer *);

int print_ptr(MetaDataBuffer * mdBuffer,ptr offset);

ptr  insertToBuffer(MetaDataBuffer *mdBuffer ,ptr headList_Node, uint32_t value ,uint32_t version);

int rec_start_print(MetaDataBuffer *mdBuffer,ptr starting_point);


#endif
