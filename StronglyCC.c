#include "StronglyCC.h"

Component *last = NULL;;

SCC* CreateSCC(uint32_t size_of_graph) {
	uint32_t i;
	SCC *scc;

	scc = malloc(sizeof(SCC));
	scc->components = NULL;
	scc->components_count = 0;
	scc->id_belongs_to_component = malloc(size_of_graph*sizeof(uint32_t));
	if(scc->id_belongs_to_component == NULL)
		return NULL;
	for(i=0;i<size_of_graph;i++)
		scc->id_belongs_to_component[i] = -1;

	return scc;
}

int InsertComponent(Component** comp, uint32_t SCC_ID, uint32_t numOfNodes, uint32_t *nodeIds) {
	uint32_t i;
	
	if(*comp == NULL) {
		*comp = malloc(sizeof(Component));
		(*comp)->component_id = SCC_ID;
		(*comp)->included_nodes_count = numOfNodes;
		(*comp)->included_node_ids = malloc(numOfNodes*sizeof(uint32_t));
		(*comp)->min_rank = 0;
		(*comp)->rank = 0;
		(*comp)->counter = 0;
		(*comp)->to_check = 0;
		//we malloc 10 pointers to component and we initialize them all to NULL
		(*comp)->c_neighbors=malloc(10*sizeof(Component *));
		for (i=0;i<10;i++)
			(*comp)->c_neighbors[i]=NULL;
		(*comp)->compon_neig_size=10;
		(*comp)->next = NULL;
		last = *comp;
		for(i=0;i<numOfNodes;i++)
			(*comp)->included_node_ids[i] = nodeIds[i];
		return 1;
	}
	else {
		Component *new = last->next;
		new = malloc(sizeof(Component));
		new->component_id = SCC_ID;
		new->included_nodes_count = numOfNodes;
		new->included_node_ids = malloc(numOfNodes*sizeof(uint32_t));
		new->min_rank = 0;
		new->rank = 0;
		new->counter = 0;
		new->to_check = 0;
		new->compon_neig_size=10;
		//we malloc 10 pointers to component and we initialize them all to NULL
		new->c_neighbors=malloc(10*sizeof(Component *));
		for (i=0;i<10;i++)
			new->c_neighbors[i]=NULL;

		new->next = NULL;
		last->next = new;
		last = new;
		for(i=0;i<numOfNodes;i++)
			new->included_node_ids[i] = nodeIds[i];
		return 1;
	}
		

	return -1;
}


SCC* estimateStronglyConnectedComponents(NodeIndex *outcoming_index,MetaDataBuffer *b_out,uint32_t megethos){
	SCC *scc;
	scc=CreateSCC(megethos);	//Create the structure
	Tarjan(outcoming_index,b_out,&scc,megethos);	//build it with iterative tarjan
	return scc;
}

int  examine_reachable_nodes_SCC(uint32_t *id_inverted,uint32_t target_Component,char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb){
	
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
					if (id_inverted[neighborI]!=target_Component)	//if the neighbor belong to another component
						continue;
					if(dirtybit_array_b[neighborI]!=1){	
						if (dirtybit_array_a[neighborI]!=1){
							// if neighbor is not -1, and not dirty at any of the two bit arrays ,make him dirty at the one array
							//and insert him to the return list of the function
							dirtybit_array_a[neighborI]=1;
						
							insert_neighbor_array(nb,neighborI);
							 
						}
						else
							//if it's dirty at the one array but not to the other continue with nexte reachable neighbor
							//printf("o %"SCNd32" vrwmiaris\n",neighborI);
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


int bfs_SCC(SCC* scc,NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors){
	
	int forward_step=0;
	int backward_step=0;
	uint32_t *id_inverted = scc->id_belongs_to_component; //access to inverted index 
	uint32_t target_component= id_inverted[nodeA];		//
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
		//change here!
		retout=examine_reachable_nodes_SCC(id_inverted,target_component,dirty_outcoming,dirty_incoming,outcomingIndexMeta,outcomingBuffer,forward_neighbors);
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
		
			retin=examine_reachable_nodes_SCC(id_inverted,target_component,dirty_incoming,dirty_outcoming,incomingIndexMeta,incomingBuffer,backward_neighbors);
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


void PrintAllComponents(Component *comp) {
	int i, counter=1;
	while (comp!=NULL){
		printf("%d: ",comp->component_id);
		counter++;
		for(i=0;i<comp->included_nodes_count;i++)
			printf("%d ",comp->included_node_ids[i]);
		printf("To paron component sinoreuei me ta parakatw: \n");
		for(i=0;i<comp->counter;i++){
			printf("(%"SCNd32"-->",(comp->c_neighbors[i])->component_id);
		}
		printf("\n");
		comp = comp->next;
	}
}

int findNodeStronglyConnectedComponentID(SCC* components, uint32_t nodeId) {
	return components->id_belongs_to_component[nodeId];
}

int iterateStronglyConnectedComponentID(SCC* scc, ComponentCursor** cursor) {
	if(scc != NULL) {
		*cursor = malloc(sizeof(ComponentCursor));
		if((*cursor) == NULL)
			return FAILURE;
		(*cursor)->component_ptr = scc->components;
		return OK_SUCCESS;
	}
	return FAILURE;
}

bool next_StronglyConnectedComponentID(ComponentCursor *cursor) {
	if((cursor->component_ptr)->next != NULL) {
		cursor->component_ptr = (cursor->component_ptr)->next;
		return OK_SUCCESS;
	}
	return FAILURE;
}

void freeComponents(Component** components) {
	Component *n;
	while (*components != NULL) {
		n = *components;
		*components = (*components)->next;
		free(n->included_node_ids);
		free(n->c_neighbors);
		free(n);
	}	
}

void destroyStronglyConnectedComponents(SCC** scc) {
	freeComponents(&((*scc)->components));
	free((*scc)->id_belongs_to_component);
	free(*scc);
}

//=========================================Tarjan=========================================//
StackCC* StackCC_Creation(){
	StackCC* S;
	S=malloc(sizeof(StackCC));
	S->size=0;
	S->start=NULL;

	return S;
}
int StackCC_Destruction(StackCC *S){
	free(S);
	S=NULL;
	
	return 1;
}

NodeCC* StackCC_Top(StackCC *S ){
	return S->start;
}

int StackCC_Push(StackCC *S , NodeCC node){
	
	NodeCC* new_node;
	//Creation and assignment of the new Stack node
	new_node=malloc(sizeof(NodeCC));
	new_node->id=node.id;
	new_node->index=node.index;
	new_node->lowlink=node.lowlink;
	new_node->caller=node.caller;
	new_node->last_buffer_block=node.last_buffer_block;
	new_node->last_buffer_entry=node.last_buffer_entry;

	if (S->size==0){///Exoume adeiea stoiva
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

int StackCC_Pop(StackCC *S){
	NodeCC* temp=S->start->next;
	
	free(S->start);
	S->size--;
	S->start=temp;
	
	return 1;
}

void StackCC_PrintNode(NodeCC* n){
	printf("[ Id: %"SCNd32",Index: %d, Lowlink: %d, Last_buffer_block: %d,Last_buffer_entry: %d ]-> \n ",n->id,n->index,n->lowlink,n->last_buffer_block,n->last_buffer_entry);
}
void StackCC_Print(StackCC *S){
	NodeCC* temp;
	temp=S->start;
	while (temp!=NULL){
		StackCC_PrintNode(temp);
		temp=temp->next;
	}
	printf("H ektipwsh oloklirwthike me epitixia\n");
}



//This function returns the next neighbor of node n in the graph,using the index-buffer structure 

uint32_t bring_me_next_neighbor(NodeCC* n,NodeIndex *indexMeta,MetaDataBuffer *mbuffer){
	list_node buffer_block;
	uint32_t *index=indexMeta->index;
	list_node *buffer=mbuffer->buffer;
	uint32_t block_no=-1;



	if (n->last_buffer_block==-2){	// if we want the first neigbor ,we take it from the index
		if ( (n->id > indexMeta->size) || (index[n->id]==-1) )
			return -1; 		// node has not neigbors	
		n->last_buffer_block=index[n->id];	//update the search_block and the last entry
		n->last_buffer_entry=0;
		
		return buffer[index[n->id]].neighbor[0]; //there is no case the first block of neighbors is empty,only if does not exist
	}

	else{		//else if we want another neigbor,we load the specific buffer block and take him
		if (n->last_buffer_entry == N-1 ){	//if we need to change block ,
			block_no=buffer[n->last_buffer_block].nextListNode;
			if (block_no==-1)	//no other neigbors return -1
				return -1;
			n->last_buffer_block=block_no;	//update the search_block and the last entry
			n->last_buffer_entry=0;
			return buffer[block_no].neighbor[0];
		}
		else{	// normal case
			block_no=n->last_buffer_block;
			n->last_buffer_entry++;
		
			if ( (buffer[block_no].neighbor[n->last_buffer_entry]) == -1){  //if we are at first block already
				
				block_no=buffer[block_no].nextListNode;
				if (block_no==-1)
					return -1;
				n->last_buffer_block=block_no;
				n->last_buffer_entry=0;
				return buffer[block_no].neighbor[0];
			}
			else
				return buffer[block_no].neighbor[n->last_buffer_entry];
			
		}
	}

}
int numCC=0;

uint32_t min_uint(uint32_t a,uint32_t b){
	if (a<=b)
		return a;
	else
		return b;
}
//Basic Tarjan Function -Small Tarjan takes an array of NodeCC structs alos
void tarjan_iter(uint32_t node,NodeCC *nodes,uint32_t *index,char *onStack,StackCC *S,char *defined_nodes,NodeIndex *indexMeta,MetaDataBuffer *mbuffer, SCC **scc){
	NodeCC *last;
	uint32_t neig, i;


	nodes[node].id=node;
	nodes[node].index=*index;
	nodes[node].lowlink=*index;
	nodes[node].last_buffer_block=-2;
	nodes[node].last_buffer_entry=-1;
	nodes[node].caller=  -1 ;
	*index=*index+1;
	defined_nodes[node]='1';
	onStack[node]='1';
	StackCC_Push(S,nodes[node]);
	last=&nodes[node];
	
	while (1){
		
		neig=bring_me_next_neighbor(last,indexMeta,mbuffer);
		
		if (neig!=-1){
			if (defined_nodes[neig]=='0'){		//if node is undefined
				nodes[neig].id=neig;
				nodes[neig].caller=  last->id;
				nodes[neig].last_buffer_block=-2;
				nodes[neig].last_buffer_entry=-1;
				nodes[neig].index= *index;
				nodes[neig].lowlink=*index;
				*index=*index+1;
				//printf("INDEX IS %d \n",*index);
				defined_nodes[neig]='1';
				StackCC_Push(S,nodes[neig]);
				onStack[neig]='1';
				last=&nodes[neig];
			}
			else if (onStack[neig]=='1')
				last->lowlink=min_uint(last->lowlink,nodes[neig].index);
		}
		else{
			 
			 if (last->lowlink==last->index){
			 	int mem_count=0;
				uint32_t comp_size = 1000;
				uint32_t *comp_array = malloc(comp_size*sizeof(uint32_t));
				uint32_t comp_pointer = 0;
			 	numCC++;
			 	
			 	uint32_t temp_id;
			 	NodeCC* start;
			 	start=StackCC_Top(S);
			 	temp_id=start->id;
				mem_count++;
				comp_array[comp_pointer] = temp_id;
				comp_pointer++;
			 	onStack[temp_id]='0';
			 	StackCC_Pop(S);
			 	start=NULL;
			 	
			 	while (1){
			 		
			 		if (temp_id==last->id)
			 			break;
			 		start=StackCC_Top(S);
			 		temp_id=start->id;
			 		mem_count++;
					if (comp_pointer < comp_size){
						comp_array[comp_pointer] = temp_id;
						comp_pointer++;
					}
					else {
						comp_size = comp_size*2;
						comp_array = realloc(comp_array,comp_size*sizeof(uint32_t));
						comp_array[comp_pointer] = temp_id;
						comp_pointer++;
					}
			 		
			 		onStack[temp_id]='0';
			 		StackCC_Pop(S);
			 		start=NULL;
			 		
			 	}
			 	
				InsertComponent(&((*scc)->components), numCC-1, comp_pointer, comp_array);
				((*scc)->components_count)++;
				for (i=0;i<comp_pointer;i++)
					(*scc)->id_belongs_to_component[comp_array[i]] = numCC -1;

				free(comp_array);
				
			}
			uint32_t newLast = last->caller;   //Go up one recursive call  koita to prosektikoterateraterateratera !! 
            if(newLast != -1) {
                nodes[newLast].lowlink = min_uint(nodes[newLast].lowlink, last->lowlink);
                last = &nodes[newLast];
            } else {   //We've seen all the nodes
                break;
            }

		}
		
	}
}


///grapse ton kwdika tou megaaaaaalou tarjaaaaaan!!!! 
void Tarjan(NodeIndex *indexMeta,MetaDataBuffer *mbuffer, SCC **scc, uint32_t size){
	int i;

	uint32_t index_size=0;


	index_size = size;


   //1.)initialize index-variable
	numCC=0;
	uint32_t index=0;
   //2.)creates the Stack;
	StackCC *S;
	S=StackCC_Creation();
   //3.)Creation of the 2 byte arrays ,one for defined nodes and one for those on stack
	char *defined_nodes,*onStack;
	defined_nodes=malloc(index_size * sizeof(char));
	onStack=malloc(index_size * sizeof(char));
	
	for(i=0;i<index_size;i++){
		defined_nodes[i]='0';
		onStack[i]='0';
	}
   //4.)Creates nodeCC array and sets it
	NodeCC *nodes;
	nodes=malloc(index_size*sizeof(NodeCC));
	for(i=0;i<index_size;i++){
		nodes[i].id=i;
		nodes[i].index=-1;
		nodes[i].lowlink=-1;
		nodes[i].caller=-1;
		nodes[i].last_buffer_block=-2;
		nodes[i].last_buffer_entry=-1;
		nodes[i].next=NULL;
	}
	//5.)run tarjan iter for each undefined node
	for(i=0;i<index_size;i++){
		if (defined_nodes[i]=='0'){
			//printf("ola kala apo to megalo\n");
			tarjan_iter(nodes[i].id,nodes,&index,onStack,S,defined_nodes,indexMeta,mbuffer,&(*scc));
		}
	}
	//6.)destruct whatever u created
	free(defined_nodes);
	free(onStack);
	free(nodes);
	free(S);


}
