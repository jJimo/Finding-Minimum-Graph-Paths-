#include "buffer.h"
 
//This function initializes the "metadata_buffer" and allocates a new buffer in the size of "SIZE_OF_BUFFER" .
//It returns NULL in case of errror.
MetaDataBuffer * createBuffer(void){
		
	MetaDataBuffer * mdBuffer ;
	
	mdBuffer=malloc(sizeof(MetaDataBuffer));
	if( mdBuffer == NULL ){
		return NULL;
	}
	mdBuffer->size = SIZE_OF_BUFFER;
	mdBuffer->insert_pos= -1;
	mdBuffer->buffer=malloc(sizeof(list_node) * SIZE_OF_BUFFER);
	if(  (mdBuffer->buffer ) == NULL  ){
		free(mdBuffer);
		return NULL;
	}	
	return 	(mdBuffer);
}


// Allocates a new node and returns an "offset" (type ptr)
	//In case of error it returns a negative number
	//Note: This function may change buffer's size,if there is not enough space for the node (buffer's size will be doubled)
ptr allocNewNode( MetaDataBuffer ** mdBuffer_){
	 MetaDataBuffer * mdBuffer = (* mdBuffer_) ;
	if( (mdBuffer->size)-1 == mdBuffer->insert_pos ){
		list_node * temp_pointer;
		mdBuffer->size= (mdBuffer->size)*2; // Doubling of buffer
		temp_pointer=realloc(mdBuffer->buffer,(sizeof(list_node) * mdBuffer->size)) ; // reallocation of memory
		if( temp_pointer != NULL ){
			
			mdBuffer->buffer = temp_pointer;
			(mdBuffer->insert_pos)++;
			
			uint32_t offset = (mdBuffer->insert_pos);	
			(mdBuffer->buffer[offset]).nextListNode=-1;
			
			int i;
			for(i=0;i<N;i++){

				(mdBuffer->buffer[offset]).neighbor[i]=-1;			//initialization of "neighbor" chart to -1.
				(mdBuffer->buffer[offset]).edgeProperty[i]=-1;	
				(mdBuffer->buffer[offset]).edgeProperty[i]=-1;		//initialization of "edgeProperty" chart to -1.
				(mdBuffer->buffer[offset]).nextListNode=-1;			//initialization of	"nextListNode" pointer to -1. (in order to be the last node).		
			}
			return (mdBuffer->insert_pos);
			
		}
		else{	
			mdBuffer->size= (mdBuffer->size)/2;//reset of the size in the previous size if realloc fails.
			return REALLOCS_ERROR;
		}
		
	}
	else{
		
		(mdBuffer->insert_pos)++;
		uint32_t offset = (mdBuffer->insert_pos);
		//initialization of "list_node" in order to be ready for use.
		
		(mdBuffer->buffer[offset]).nextListNode=-1;
		
		int i;
		for(i=0;i<N;i++){

			(mdBuffer->buffer[offset]).neighbor[i]=-1;			//initialization of "neighbor" chart to -1.
			(mdBuffer->buffer[offset]).edgeProperty[i]=-1;	
			(mdBuffer->buffer[offset]).edgeProperty[i]=-1;		//initialization of "edgeProperty" chart to -1.
			(mdBuffer->buffer[offset]).nextListNode=-1;			//initialization of	"nextListNode" pointer to -1. (in order to be the last node).
			
		}
		
		return (mdBuffer->insert_pos);
		
	}
	
	
	return 0;
}


//This function returns a "list_node",structure, which is indicated by the ptr (offset).
	//This function returns "Null" if the given ptr is not corresponding to a "list_node"
	//WARNING: This "list node" is only for reading.
list_node * getListNode(MetaDataBuffer * mdBuffer,ptr offset){
	if(mdBuffer->insert_pos  >= offset)
		return &(mdBuffer->buffer[offset]);	
	else
		return NULL; //offset out of range
}




//It destroys the buffer. ( OK SUCCESS==1 in case of success or -1 in case of failure)
int destroyBuffer(MetaDataBuffer * mdBuffer){
	if( (mdBuffer->buffer) != NULL ){
		free(mdBuffer->buffer);
	}
	else
		return METABUFFERS_ERROR;
		
	if( mdBuffer != NULL ){
		free(mdBuffer);
	}
	else
		return BUFFERS_ERROR;
		
	return OK_SUCCESS;
}

//initially it checks if we have space on mdBuffer->buffer[headList_Node].If so,we enter the value there and we return OK_SUCCESS
//else we allocate a New Node,we enter the value and then we return to index the new ptr of the head of listnode list for the specific vertex  
//WE PERFORM INSERTIONS ONLY AT THE START OF THE LIST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//additional function which checks and returns if an empty position exists in neighbor array of the listnode
int have_space(list_node L){
	int i,first_free=-1;
	for (i=0;i<N;i++){
		if (L.neighbor[i]== -1){		// if i find space i return it
			first_free=i;
			break;
		}
	}
	return first_free;
}



//"insertToBuffer" is a function which assignes a "value" into a buffer
// The value will be added in the node which "ptr" indicates.(ptr could have been created from a previous calling of this function)
//IMPORTANT: When there is not a "ptr" to call this function we set "ptr" (headList_Node) to -1.
	//This usually happens the first time that we want to add a new "neighbor" to a new node(value).
//The function returns the "offset" in which it wrote the "value".	
ptr  insertToBuffer(MetaDataBuffer *mdBuffer ,ptr headList_Node, uint32_t value ,uint32_t version){
	int pos=-2;
	uint32_t new_pos=-2;

	if(headList_Node==-1){
		new_pos=allocNewNode(&mdBuffer);			//produce a new block
		if(new_pos == REALLOCS_ERROR) 
			return REALLOCS_ERROR;
			
		(mdBuffer->buffer[new_pos]).neighbor[0]=value;			//set value
		(mdBuffer->buffer[new_pos]).edgeProperty[0]=version;
		return new_pos;		
	}
	pos=have_space(mdBuffer->buffer[headList_Node]);		//search only the starting block
	if (pos!=-1){
		(mdBuffer->buffer[headList_Node]).neighbor[pos]=value;
		(mdBuffer->buffer[headList_Node]).edgeProperty[pos]=version;
		return headList_Node;
	}
	else{
		new_pos=allocNewNode(&mdBuffer);			//produce a new block
		if(new_pos == REALLOCS_ERROR) 
			return REALLOCS_ERROR;
			
		(mdBuffer->buffer[new_pos]).nextListNode = headList_Node;	//modify the internal representation 
		(mdBuffer->buffer[new_pos]).neighbor[0]=value;			//set value
		(mdBuffer->buffer[new_pos]).edgeProperty[0]=version;		
		
		return new_pos;			//returns the new starting ptr for the vertex

	}
}
 

 //Retrospective function which prints all of the nodes of the chain. 
int rec_start_print(MetaDataBuffer *mdBuffer,ptr starting_point){
	if(starting_point < mdBuffer->insert_pos)
		return PTR_OUT_OF_RANGE;
		
	if (mdBuffer->insert_pos!=-1){
		int i;
		struct list_node l;
		l=mdBuffer->buffer[starting_point];
		if ( l.nextListNode != -1){
			for (i=N-1;i>=0;i--){
				if (l.neighbor[i]!=-1)
					printf("% "SCNd32"->",l.neighbor[i]);
			}
			return rec_start_print( mdBuffer,l.nextListNode);
		}
		else
			for (i=N-1;i>=0;i--){
				if (l.neighbor[i]!=-1)
					printf("% "SCNd32"->",l.neighbor[i]);
			}
			printf("\n");
			return 1;
	}
	else{
		printf("I cannot print an empty buffer\n");
		return EMPTY_BUFFERS_ERROR;
	}
}


//Print all nodes in a ptr.
int print_ptr(MetaDataBuffer * mdBuffer,ptr offset){
	int i;
	if(offset <= (mdBuffer->insert_pos) ){	//if list_node with specific offset exist.

		int temp_offset = offset;
		while((temp_offset <= mdBuffer->insert_pos) && (temp_offset!=-1)){		//for every node
			printf("oi gitones einai: ");
			for(i=0;i<N;i++){
				
				uint32_t x = (mdBuffer->buffer[temp_offset]).neighbor[i];
				printf(" % "SCNd32" , ",x);
			}
			printf("\n");
			
			temp_offset = (mdBuffer->buffer[temp_offset]).nextListNode;		//Go to the next node
			printf("\n{%d}//%d\n",temp_offset ,mdBuffer->insert_pos);
		}
	}
	else{	//if "list_node" with specific offset does not exist.
		
		return PTR_OUT_OF_RANGE;
	}

	return 0;
}



