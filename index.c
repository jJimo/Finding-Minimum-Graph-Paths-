#include "index.h"

NodeIndex* createNodeIndex() {
	NodeIndex *new_index;													//New struct index pointer
	int i;

	new_index = malloc(sizeof(NodeIndex));									//Allocate memory for struct index
	new_index->size = DEFAULT_INDEX_SIZE;									//Set size of index to DEFAULT_INDEX_SIZE
	new_index->index = malloc(DEFAULT_INDEX_SIZE*sizeof(uint32_t));			//Allocate memory for index
	
	for(i=0;i<DEFAULT_INDEX_SIZE;i++)										//Initialize index 
		new_index->index[i] = -1;

	return new_index;														//Return pointer of struct index
	
}

int destroyNodeIndex(NodeIndex *ind) { 										//Destroy an index
	free(ind->index);														//Free index
	free(ind);																//Free struct index

	return OK_SUCCESS;														//Return OK_SUCCESS 
}

int increaseIndex(NodeIndex *ind, uint32_t node) {							//Increase index
	uint32_t i = ind->size, *new_index = NULL;								//i=previous_index_size

	if(node > 2*i) {														//If new node is greater than previous index size
		ind->size = node+1;													//Make new index size = node
		new_index = realloc(ind->index, (node+1)*sizeof(uint32_t));			//Reallocate index
		if(new_index == NULL)												//Return error if realloc fails
			return -1;

		ind->index = new_index;
		for(i;i<ind->size;i++)												//Initialize new nodes
			ind->index[i] = -1;	
	}
	else {
		ind->size = 2*i;													//Reduplicate index size
		new_index = realloc(ind->index, 2*i*sizeof(uint32_t));				//Reallocate index
		if(new_index == NULL)
			return FAIL;

		ind->index = new_index;
		for(i;i<ind->size;i++)
			ind->index[i] = -1;												//Initialize new nodes 

	}

	return OK_SUCCESS;														//Return ok_success
}
 
int insertNode(NodeIndex *ind, MetaDataBuffer *buffer, uint32_t new_node , uint32_t edge, uint32_t version) {
	int err;

	while(ind->size <= new_node) {
		err = increaseIndex(ind, new_node);
		if(err != 1)
			return FAIL;
	}
	
	ind->index[new_node] = insertToBuffer(buffer ,ind->index[new_node], edge , version);		//Insert edge to buffer and set value of index
	if ( (ind->index[new_node]) == -1 )
		return FAIL;
																						
	return OK_SUCCESS;																//Return OK_success
}

ptr getListHead(NodeIndex *ind, uint32_t nodeId) {
	if(nodeId <= ind->size)
		return ind->index[nodeId];

	return FAIL;
}

void printIndex(NodeIndex *ind, MetaDataBuffer *buffer) {
	int i;
	
	for(i=0;i<ind->size;i++)
		if(ind->index[i] != -1) {											//If node exists
			printf("Index : %d \n",i);										//Print node and neighbors
			rec_start_print(buffer,ind->index[i]);
			
		}
}


