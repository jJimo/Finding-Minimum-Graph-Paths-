#ifndef bit_array
#define bit_array

#include <math.h>
#include <limits.h>

#define DIRTY 1
#define NO_DIRTY 0 
#define SUCCESS 1


char * create_bit_array(uint32_t index_size);		//function that creates the bit array
int  destroy_bit_array(char *b);					//function which performs the free function to the bit_array
int make_dirty(char *b,uint32_t node);				//function that makes bit assigned to node dirty (§§ which indicates that node has been visited)
int check_dirty(char *b,uint32_t node);				//function which checks if bit assigned to node is 1 (meaning dirty)
	
#endif
