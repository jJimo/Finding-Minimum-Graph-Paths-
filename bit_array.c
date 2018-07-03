#include "list.h"
#include "bit_array.h"
 

char * create_bit_array(uint32_t index_size){
	char *b;
	uint32_t array_size=0;
	int i;

	//if i have 'index_size'-nodes and assign 1 bit per node,i have to malloc (index_size/CHAR_BIT * sizeof(char) ) positions
	array_size=(index_size/CHAR_BIT)+1;
	b=malloc(array_size*sizeof(char));

	for (i=0;i<array_size;i++)
		b[i]=0;		//initialize all bits to zero
	
	
	return b;
}

int destroy_bit_array(char *b){
	//just release the memory
	free(b);
	return SUCCESS ;
}

int make_dirty(char *b,uint32_t node){
	uint32_t p,which_byte,which_bit;
	uint32_t t;
	which_byte= node / CHAR_BIT;		//find the byte(i.g. the position in the bit_array) which contains the bit assigned to the node
	which_bit=  node % CHAR_BIT ;		//find the bit assigned to the node
	t=pow(2.0,which_bit);				//prepare the mask ,a number with only one 1 
	b[which_byte]= b[which_byte] |t;	//to change the bit perform bitwise or with the mask
	return SUCCESS;
}

int check_dirty(char *b,uint32_t node){
	uint32_t p,which_byte,which_bit;
	uint32_t t;
	which_byte= node / CHAR_BIT;		//find the byte(i.g. the position in the bit_array) which contains the bit assigned to the node
	which_bit=  node % CHAR_BIT ;		//find the bit assigned to the node
	t=pow(2.0,which_bit);				//prepare the mask ,a number with only one 1
	if ( (b[which_byte] & t) == 0)		//if bitwise AND of the specific byte with mask is zero that means that the bit is not dirty,else is.
		return NO_DIRTY;				//cause the mask is a binary number with only one 1.therefore,bitwise and gives zero to all other bits for sure.
	else								//if the examined bit is 1 then AND gives a not-zero number.if it is 0 AND finally gives a zero-number.
		return DIRTY;
}
