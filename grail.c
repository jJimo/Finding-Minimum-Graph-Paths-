#include "grail.h"

void constructHyperGraph(SCC **scc, NodeIndex *ind, MetaDataBuffer *out) {
	uint32_t i,j,k;
	uint32_t offset;
	Component *comp = (*scc)->components;
	uint32_t *id_inverted = (*scc)->id_belongs_to_component;
	char *component_id_neighbors; //gets cleaned each time
	int metritis=1;
	component_id_neighbors=malloc((*scc)->components_count *sizeof(char));
	Component **comp_hash = createComponentsArray(*scc);
	
	for (i=0;i<(*scc)->components_count;i++)
		component_id_neighbors[i]='0';
	while(comp != NULL) {

		for(i=0 ; i< (comp->included_nodes_count) ; i++) {
			if(ind->size > comp->included_node_ids[i])
				offset = ind->index[comp->included_node_ids[i] ];
			if(offset != -1) {
				do {
					for(j=0;j<N;j++) {
						if( ((out->buffer[offset]).neighbor[j]) == -1)
							break;
						
						if( (id_inverted[comp->included_node_ids[i] ] ) != (id_inverted[(out->buffer[offset]).neighbor[j] ]) ) {	//if it belongs to different SCC
							uint32_t lefterakos = id_inverted[(out->buffer[offset]).neighbor[j]];
							if (component_id_neighbors[lefterakos]=='1'){
								continue;
							}
							else if (comp->counter < comp->compon_neig_size) { 	
								comp->c_neighbors[comp->counter] = comp_hash[ id_inverted[(out->buffer[offset]).neighbor[j]] ];
								(comp->counter)++;
								component_id_neighbors[id_inverted[(out->buffer[offset]).neighbor[j]]]='1';
							}
							else{
								comp->compon_neig_size=2*(comp->compon_neig_size);
								comp->c_neighbors=realloc(comp->c_neighbors,comp->compon_neig_size*sizeof(Component *));
								comp->c_neighbors[comp->counter] = comp_hash[ id_inverted[(out->buffer[offset]).neighbor[j]] ];	
								(comp->counter)++;
								component_id_neighbors[id_inverted[(out->buffer[offset]).neighbor[j]]]='1';
							}
						}
					}
					offset = (out->buffer[offset]).nextListNode;
				}while(offset != -1);
			}
		}
	    metritis++;
		for (i=0;i<comp->counter;i++)
			component_id_neighbors[comp->c_neighbors[i]->component_id]='0';	
		comp = comp->next;	
	}
	free(component_id_neighbors);
	free(comp_hash);
}

Component **createComponentsArray(SCC *scc) {
	Component **hash_comp = malloc((scc->components_count) * sizeof(Component *));
	uint32_t i;
	Component *comp = scc->components;

	for(i=0;i<scc->components_count;i++)
		hash_comp[i] = NULL;											

	while(comp != NULL) {
		hash_comp[comp->component_id] = comp;
		comp = comp->next;
	}

	return hash_comp;
}

uint32_t rank = 1;
//second grail

uint32_t rank2 = 1;

void buildGrailIndex(SCC *scc) {
	char *byte_array;
	uint32_t i;
	int k = scc->components_count;

	Component **comp_hash = createComponentsArray(scc);
	byte_array = malloc(k*sizeof(char));
	for(i=0;i<k;i++)
		byte_array[i] = '0';
//first grail
	for(i=0;i<k;i++)
		if(byte_array[i] == '0')
			createRanks(scc, comp_hash, byte_array, i);	
//second grail
	for(i=0;i<k;i++)
		byte_array[i] = '0';
	for(i=0;i<k;i++)
		if(byte_array[i] == '0')
			createRanks2(scc, comp_hash, byte_array, i);

	free(comp_hash);
	free(byte_array);
}

int createRanks(SCC *scc,Component **hash, char *byte_array,  uint32_t start) {
	uint32_t i, min = 0;
	Component *n = hash[start], *prev = NULL; 
	StackPOT *s = StackPOT_Creation();
	NodePOT *tmp;
	byte_array[start] = '1';
	
	if( StackPOT_Push(s , n) == -1 )
		return -1;

	do {
		if(n->to_check >= n->counter) {
			if(n->counter == 0){
				n->min_rank = rank;
				n->rank = rank;
				rank++;
			}
			else{
				for(i=0;i<n->counter;i++) {								//find min_rank of neighbors
					
					if(n->c_neighbors[i]->min_rank != 0) {
						if(min == 0) {
							min = n->c_neighbors[i]->min_rank;
						}
						else if((n->c_neighbors[i])->min_rank < min)
							min = n->c_neighbors[i]->min_rank;
					}
				}
				n->min_rank = min;
				n->rank = rank;
				min = 0;
				rank++;
			}
			
			StackPOT_Pop(s);					//pop it from the stack
			tmp=StackPOT_Top(s);				//find top node of stack
			if(tmp == NULL)
				break;
			n=tmp->comp;						//take the component pointer
		}
		else {
			prev = n;
			n = n->c_neighbors[n->to_check];
			(prev->to_check)++;
			if(byte_array[n->component_id]  == '0'){
				StackPOT_Push(s, n);
				byte_array[n->component_id] = '1';
			}
			else{
				n = prev;
			}
		}
	}while(s->size != 0);

	StackPOT_Destruction(s);
	return 0;
}

// §§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§ SECOND GRAIL : CREATE RANKS_2 §§§§§§§§§§§§§§§§§§§§§§§§§§§§§

int createRanks2(SCC *scc,Component **hash, char *byte_array,  uint32_t start) {
	uint32_t i, min = 0;
	Component *n = hash[start], *prev = NULL; 
	StackPOT *s = StackPOT_Creation();
	NodePOT *tmp;
	byte_array[start] = '1';
	
	if( StackPOT_Push(s , n) == -1 )
		return -1;

	do {
		if(n->to_check >= n->counter) {
			if(n->counter == 0){
				n->min_rank2 = rank2;
				n->rank2 = rank2;
				rank2++;
			}
			else{
				for(i=0;i<n->counter;i++) {								//find min_rank of neighbors
					
					if(n->c_neighbors[i]->min_rank2 != 0) {
						if(min == 0) {
							min = n->c_neighbors[i]->min_rank2;
						}
						else if((n->c_neighbors[i])->min_rank2 < min)
							min = n->c_neighbors[i]->min_rank2;
					}
				}
				n->min_rank2 = min;
				n->rank2 = rank2;
				min = 0;
				rank2++;
			}
			
			StackPOT_Pop(s);					//pop it from the stack
			tmp=StackPOT_Top(s);				//find top node of stack
			if(tmp == NULL)
				break;
			n=tmp->comp;						//take the component pointer
		}
		else {
			prev = n;
			n = n->c_neighbors[(n->counter-1)-n->to_check];
			(prev->to_check)++;
			if(byte_array[n->component_id]  == '0'){
				StackPOT_Push(s, n);
				byte_array[n->component_id] = '1';
			}
			else{
				n = prev;
			}
		}
	}while(s->size != 0);

	StackPOT_Destruction(s);
	return 0;
}

GRAIL_ANSWER isReachableGrailIndex(SCC *scc, Component **hash, uint32_t source_node,uint32_t target_node) {
	uint32_t *id_inverted = scc->id_belongs_to_component;
	if(id_inverted[source_node] == id_inverted[target_node])
		return YES;

	Component *source = hash[id_inverted[source_node]];
	Component *target = hash[id_inverted[target_node]];
	if( (source->min_rank <= target->min_rank) && (target->min_rank <= source->rank) && (source->min_rank <= target->rank) && (target->rank <= source->rank) ){
		if (! (source->min_rank2 <= target->min_rank2) && (target->min_rank2 <= source->rank2) && (source->min_rank2 <= target->rank2) && (target->rank2 <= source->rank2) )
			return NO;
		else
			return MAYBE;
	}
	return NO;
}
// Sinartiseis stoivas gia thn Post-Order Diasxish

StackPOT* StackPOT_Creation(){
	StackPOT* S;
	S=malloc(sizeof(StackPOT));
	S->size=0;
	S->start=NULL;
	//printf("StackPOT CREATED SUCCESFULLY\n");
	return S;
}

int StackPOT_Destruction(StackPOT *S){
	free(S);
	S=NULL;
	//printf("StackPOT SUCCESFULLY DELETED\n");
	return 1;
}

NodePOT* StackPOT_Top(StackPOT *S ){
	if (S->size==0)
		return NULL;	
	return S->start;
}

int StackPOT_Push(StackPOT *S , Component *node){

	NodePOT* new_node;
	//Creation and assignment of the new Stack node
	new_node=malloc(sizeof(NodePOT));
	new_node->comp = node;		//pass of data 
	
	if (S->size==0){///Exoume adeia stoiva
		S->start=new_node;
		new_node->next=NULL;
		S->size=1;
	}
	else{
		new_node->next=S->start;
		S->start=new_node;
		S->size++;
	}	
	return 1;
}

int StackPOT_Pop(StackPOT *S){
	NodePOT* temp=S->start->next;
	free(S->start);
	S->size--;
	S->start=temp;
	
	return 1;
}

void StackPOT_Print(StackPOT *S){
	NodePOT* temp;
	temp=S->start;
	while (temp!=NULL){
		printf("%"SCNd32"-> ",temp->comp->component_id);
		temp=temp->next;
	}
	printf("\nPrint comp[leted successfully.\n");
}



// --------------------------------- BFS ME GRAIL STO MAYBE ----------------------------------------


int  examine_reachable_nodes_GRAIL(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb,SCC *scc,Component **hash,uint32_t node){
	
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
				//printf(" geitonas % "SCNd32"\n",neighborI);
				if(neighborI != -1){
					if (isReachableGrailIndex(scc,hash,neighborI,node) == NO){
						//dirtybit_array_a[neighborI]=1;
						continue;
					}
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
int bfs_GRAIL(NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors,SCC *scc,Component **hash){
	
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
		retout=examine_reachable_nodes_GRAIL(dirty_outcoming,dirty_incoming,outcomingIndexMeta,outcomingBuffer,forward_neighbors,scc,hash,nodeB);
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

			retin=examine_reachable_nodes(dirty_incoming,dirty_outcoming,incomingIndexMeta,incomingBuffer,backward_neighbors);
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



