#ifndef Neigb
#define Neigb

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#define OK_SUCCESS 1
#define REALLOC_ERROR_DIRTYBYTE_ARRAY -1

typedef struct neighbors{
	
	uint32_t *nodes;
	uint32_t elements_in;
	uint32_t start_from_here;
	uint32_t neighbor_size;
	
}neighbors;


typedef struct DirtyByteArray{
	
	int size_dirtybyte_array;
	char * array;
	
}DirtyByteArray;

 neighbors *create_neighbor_array(void);
 
 DirtyByteArray *create_dirtybyte_array(uint32_t index_size);
 
 int destroy_neighbor_array(neighbors *n);

 int destroy_dirtybyte_array(DirtyByteArray *db);
 		
 int sanitize_structures(neighbors *n,char * db_array );
 
 int insert_neighbor_array(neighbors *n,uint32_t node);
 
 int realloc_dirtybyte_array(DirtyByteArray * db,uint32_t size_index_new);


#endif
