#include  "bfs.h"



int  examine_reachable_nodes_version(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb,uint32_t query_version){

	int j;
	uint32_t elements_inside=nb->elements_in;
	uint32_t *index = indexMeta->index; 	//access to the index
	list_node * buffer = mBuffer->buffer; 	//access to the buffer

	for (j=nb->start_from_here;j<elements_inside;j++){		//Okei bitchez,now let's look for neighbors really fast

		if(index[nb->nodes[j]] == -1 )  //if any node in array has  not any neighbors we continue the loop
			continue;

		uint32_t currentNode = index[nb->nodes[j]];	//else take the number of its first block in buffer from the index
		do{		//for each of this node's neighbors do . . .
			int i;
			for(i=0;i<N;i++){
			
				uint32_t neighborI= buffer[currentNode].neighbor[i]; 		//again,for simplicity reasons
				
				if(neighborI != -1){
					if(buffer[currentNode].edgeProperty[i] <= query_version){//expand the node only if (edge.version <= query.version)
						
						if(dirtybit_array_b[neighborI]!=1){	
							if (dirtybit_array_a[neighborI]!=1){
								// if neighbor is not -1, and not dirty at any of the two bit arrays ,make him dirty at the one array
								//and insert him to the return list of the function
								dirtybit_array_a[neighborI]=1;

								insert_neighbor_array(nb,neighborI);
					 
							}
							else
								//if it's dirty at the one array but not to the other continue with nexte reachable neighbor
								continue;
						}
						else{
						
							//if neighbor has already bean reached by the other-direction bfs ,and is reaching by that bfs
							//we have found path,the minimum path
							//So,we return NULL instead of list ,and make the state have value the step of newList
							//finally we destroy what we previously malloced,as well as oldlist passed as an argument
							return FOUND_PATH;
						}
					}				
				}
				else
					break;
			}
			currentNode=buffer[currentNode].nextListNode;  //we continue searching to the next block of the buffer
		}while (currentNode!=-1);
		nb->start_from_here=elements_inside; 
		
	}
	
	if (elements_inside==nb->elements_in)
		return NO_PATH;
	return MOVE_ON;
}





int  examine_reachable_nodes(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb){

	int j;
	uint32_t elements_inside=nb->elements_in;
	uint32_t *index = indexMeta->index; 	//access to the index
	list_node * buffer = mBuffer->buffer; 	//access to the buffer

	for (j=nb->start_from_here;j<elements_inside;j++){		//Okei bitchez,now let's look for neighbors really fast

		if(index[nb->nodes[j]] == -1 )  //if any node in array has  not any neighbors we continue the loop
			continue;

		uint32_t currentNode = index[nb->nodes[j]];	//else take the number of its first block in buffer from the index
		do{		//for each of this node's neighbors do . . .
			int i;
			for(i=0;i<N;i++){
			
				uint32_t neighborI= buffer[currentNode].neighbor[i]; 		//again,for simplicity reasons
				if(neighborI != -1){
						
						if(dirtybit_array_b[neighborI]!=1){	
							if (dirtybit_array_a[neighborI]!=1){
								// if neighbor is not -1, and not dirty at any of the two bit arrays ,make him dirty at the one array
								//and insert him to the return list of the function
								dirtybit_array_a[neighborI]=1;

								insert_neighbor_array(nb,neighborI);
					 
							}
							else
								//if it's dirty at the one array but not to the other continue with nexte reachable neighbor
								continue;
						}
						else{
							//printf("Krisimos geitonas % "SCNd32"\n",neighborI);
							//if neighbor has already bean reached by the other-direction bfs ,and is reaching by that bfs
							//we have found path,the minimum path
							//So,we return NULL instead of list ,and make the state have value the step of newList
							//finally we destroy what we previously malloced,as well as oldlist passed as an argument
							return FOUND_PATH;
						}
				
				}
				else
					break;
						
				
			}
			currentNode=buffer[currentNode].nextListNode;  //we continue searching to the next block of the buffer
		}while (currentNode!=-1);
		nb->start_from_here=elements_inside; 
	}

	if (elements_inside==nb->elements_in)
		return NO_PATH;
	return MOVE_ON;
}


int bfs(NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,uint32_t query_version, DirtyByteArray * dirty_incomingPtr,DirtyByteArray * dirty_outcomingPtr,neighbors* forward_neighbors,neighbors* backward_neighbors){ 

	char * dirty_incoming=dirty_incomingPtr->array;
	char* dirty_outcoming=dirty_outcomingPtr->array;
	
	int forward_step=0;
	int backward_step=0;
	//now.i make 2 dirty_bit arrays clean and clean the neigbor arrays and I INSERT NODES A AND B
	int i;
	int result=-5;
	
	//set metadata for neighbor arrays
	forward_neighbors->nodes[0]=nodeA;
	forward_neighbors->elements_in=1;
	forward_neighbors->start_from_here=0;

	backward_neighbors->nodes[0]=nodeB;
	backward_neighbors->elements_in=1;
	backward_neighbors->start_from_here=0;


	//in order to find minimum paths of length B
	dirty_incoming[nodeB]=1;
	
	int retout=0;
	int retin=0;

	while(1){
		retout=examine_reachable_nodes_version(dirty_outcoming,dirty_incoming,outcomingIndexMeta,outcomingBuffer,forward_neighbors,query_version);
		forward_step++;
		if (retout==NO_PATH){
			result = -1;
			break;
		}
		else if (retout == FOUND_PATH){
			result = backward_step+forward_step;
			break;
		}
		else if (retout== MOVE_ON){		
			retin=examine_reachable_nodes_version(dirty_incoming,dirty_outcoming,incomingIndexMeta,incomingBuffer,backward_neighbors,query_version);
			backward_step++;
			if (retin == NO_PATH){
				result = -1;
				break;
			}
			else if  (retin == FOUND_PATH){
				result = backward_step+forward_step;
				break;
			}
		}
	}
	sanitize_structures(forward_neighbors,dirty_outcoming);
	sanitize_structures(backward_neighbors,dirty_incoming);
	return result;
}






