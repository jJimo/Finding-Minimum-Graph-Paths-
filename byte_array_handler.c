#include "byte_array_handler.h"

 neighbors *create_neighbor_array(void){
 	neighbors *n;
 	int u;

 	n = malloc(sizeof(neighbors));
 	if (n==NULL)
 		return NULL;

 	n->nodes = malloc(500*sizeof(uint32_t));
 	if (n->nodes == NULL)
 		return NULL;

 	n->start_from_here = 0;
 	n->elements_in = 0;
 	n->neighbor_size = 500;

 	for (u=0;u<500;u++)		//initial size is  500
 		n->nodes[u]=-1;

 	return n;
 }


 DirtyByteArray *create_dirtybyte_array(uint32_t index_size){
 	int i;
 	DirtyByteArray * db;
	
 	db = malloc(sizeof(DirtyByteArray));
 	 if (db== NULL)
 		return NULL;
 		
 	db->array = malloc(index_size *sizeof(char));
 	if (db->array == NULL)
 		return NULL;
 	for (i=0;i<index_size;i++)
 		db->array[i] = 0;

 	return db;
 }


 int destroy_neighbor_array(neighbors *n){
 	free(n->nodes);
 	free(n);
 	return OK_SUCCESS;
 }

 int destroy_dirtybyte_array(DirtyByteArray *db){
	 
 	free(db->array);
 	free(db);
 	
 	return OK_SUCCESS;
 }


 //////////////////////////////------------- SANITIZE FUNCTION --------------------- /////////////////////////

 int sanitize_structures(neighbors *n,char * db_array ){
 	uint32_t i;
 	//set specific bytes equal to 0
 	for (i=0;i<n->elements_in;i++){
 		db_array[n->nodes[i]] = '0';
 	}
 	//modify other metadata for neighor struct
	n->elements_in=0;
	n->start_from_here=0;
 	
 	return OK_SUCCESS;
 }


//////////////////////////////------------------ INSERT TO NEIGHBOR ARRAY -----------------------//////////////////////////////
 int insert_neighbor_array(neighbors *n,uint32_t node){
 	//uint32_t *new_nodes;
 	if (n->elements_in >= n->neighbor_size){		//if i need a realloc i perform it
 		n->nodes = realloc(n->nodes,n->neighbor_size*2*sizeof(uint32_t));
 		//n->nodes = new_nodes;
 		n->neighbor_size = 2*(n->neighbor_size);
 	}
 	//put new element and exit
 	n->nodes[n->elements_in]= node;
	n->elements_in++;
	return OK_SUCCESS;
}


int realloc_dirtybyte_array(DirtyByteArray * db,uint32_t size_index_new){
	
	int i;
 	char * new_dirty_array;
 	new_dirty_array = realloc((db->array),size_index_new *sizeof(char));
 	if (new_dirty_array == NULL)
 		return REALLOC_ERROR_DIRTYBYTE_ARRAY;
 		
 	db->array = new_dirty_array;
 	
 	for (i=(db->size_dirtybyte_array);i<size_index_new;i++)
 		db->array[i] = 0;
 		
 		
 	db->size_dirtybyte_array=size_index_new;
 	

 	return OK_SUCCESS;
	
}





