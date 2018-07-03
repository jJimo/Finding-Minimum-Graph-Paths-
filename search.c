#include "index.h"


int search(NodeIndex *ind, MetaDataBuffer *bfr, uint32_t start, uint32_t end) {
	uint32_t offset;
	int i;

	if( (ind->size > start) && (ind->index[start] != -1) ) {					//If "start" isn's greater than index size and index[start] isn't empty
		offset = ind->index[start];												//For all edges
		do {
			for(i=0;i<N;i++) {													//Look for end
				if( ((bfr->buffer[offset]).neighbor[i]) == end)					//If end exists
					return OK_SUCCESS;											//Return OK_SUCCESS(edge found)
				else if( ((bfr->buffer[offset]).neighbor[i]) == -1)				//If neighbor is -1 go to next bucket
					break;
			}

			offset = (bfr->buffer[offset]).nextListNode;								
		} while(offset != -1);

	}

	return FAIL;																//If edge not found, return FAIL(edge not found)
}
