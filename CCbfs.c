#include "CCbfs.h"
#define MOVE_ON 1
#define FOUND_PATH 2
#define NO_PATH -1




int  examine_reachable_nodes_DYNAMIC(char * dirtybit_array_a,char * dirtybit_array_b,CC *cc, neighbors* nb){	
	int j;
	uint32_t elements_inside=nb->elements_in, offset;
	UpdateIndex *ui = cc->updateIndex;

	for (j=nb->start_from_here;j<elements_inside;j++){		//Ok, now let's look for neighbors really fast

		offset = 0;
		if(ui->ccUpdate[nb->nodes[j]]->ccTable == NULL){
			continue;
		}
		uint32_t currentNode = ui->ccUpdate[nb->nodes[j]]->ccTable[offset];	//else take the number of its first block in buffer from the index
		while (offset != ui[nb->nodes[j]].numOfRecs){		//for each of this node's neighbors do . . .
			int i;
			
					if(dirtybit_array_b[currentNode]!=1){	
						if (dirtybit_array_a[currentNode]!=1){
							// if neighbor is not -1, and not dirty at any of the two bit arrays ,make him dirty at the one array
							//and insert him to the return list of the function
							dirtybit_array_a[currentNode]=1;
						
							nb->nodes[nb->elements_in]=currentNode;
							nb->elements_in++;
							 
						}
					}
					else{
						
						//if neighbor has already bean reached by the other-direction bfs ,and is reaching by that bfs
						//we have found path,the minimum path
						//So,we return NULL instead of list ,and make the state have value the step of newList
						//finally we destroy what we previously malloced,as well as oldlist passed as an argument
						return FOUND_PATH;
					}
			
			offset++;
			currentNode=ui->ccUpdate[nb->nodes[j]]->ccTable[offset];  //we continue searching to the next block of the buffer
		}
		nb->start_from_here=elements_inside; 
		
	}
	
	if (elements_inside==nb->elements_in)
		return NO_PATH;
	return MOVE_ON;
}


int bfs_CC(CC *cc, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors){
	if(nodeA == nodeB)
		return 1;
	int forward_step=0;
	int backward_step=0;
	//now.i make 2 dirty_bit arrays clean and clean the neigbor arrays and I INSERT NODES A AND B
	int i;
	for (i=0;i<forward_neighbors->elements_in;i++){
		dirty_outcoming[forward_neighbors->nodes[i]]=0;
		
		//forward_neighbors->nodes[i]=0;
	}
	forward_neighbors->nodes[0]=nodeA;
	forward_neighbors->elements_in=1;
	forward_neighbors->start_from_here=0;

	for (i=0;i<backward_neighbors->elements_in;i++){
		dirty_incoming[backward_neighbors->nodes[i]]=0;
		
		//backward_neighbors->nodes[i]=0;
	}
	backward_neighbors->nodes[0]=nodeB;
	backward_neighbors->elements_in=1;
	backward_neighbors->start_from_here=0;


	//in order to find minimum paths of length B
	dirty_incoming[nodeB]=1;
	
	int retout=0;
	int retin=0;

	while(1){
		retout=examine_reachable_nodes_DYNAMIC(dirty_outcoming,dirty_incoming,cc,forward_neighbors);
		forward_step++;
		if (retout==NO_PATH){
			
			return -1;
		}
		else if (retout == FOUND_PATH){
			return backward_step+forward_step;
		}
		else if (retout== MOVE_ON){
			retin=examine_reachable_nodes_DYNAMIC(dirty_incoming,dirty_outcoming,cc,backward_neighbors);
			backward_step++;
			if (retin == NO_PATH)
				return -1;
			else if  (retin == FOUND_PATH)
				return backward_step+forward_step;
		}
	}
}
