#include "bloomFilter.h"
#include <string.h>
char * createBloomFilter(void){
	char * bf;
	bf=malloc( BF_SIZE * sizeof(char) );
	if(bf==NULL){
		perror("Error sto createBloomFilter() \n");
		return NULL;
	}
	
	memset(bf,0,BF_SIZE);

	return bf;
}

void removeBloomFilter(char * bf){
	free(bf);
}

int bloomSet(char * bf,uint32_t a,uint32_t b){
	
	int p1 = h1(a,b);
	int p2 = h2(a,b);
	int p3 = h3(a,b);
	
	p1 = p1%BF_SIZE;
	p2 = p2%BF_SIZE;
	p3 = p3%BF_SIZE;
	
	bf[p1]=1;
	bf[p2]=1;
	bf[p3]=1;
	return 0;
}

int bloomCheck(char * bf,uint32_t a,uint32_t b){
	
	int p1 = h1(a,b);
	int p2 = h2(a,b);
	int p3 = h3(a,b);
	
	p1 = p1%BF_SIZE;
	p2 = p2%BF_SIZE;
	p3 = p3%BF_SIZE;
	
	if((bf[p1]==1)&&(bf[p2]==1)&&(bf[p3]==1)){
		return BF_MAYBE;
	}
	else{
		return BF_NO;
	}
}

int h1(uint32_t a,uint32_t b){
	int retval = (a + b) * (a + b + 1) / 2 + a;
	if(retval < 0)
		return -1*retval;

	return retval;
}

uint32_t h2(uint32_t a,uint32_t b){
	
	int retval = a >= b ? a * a + a + b : a + b * b;
	if(retval < 0)
		return -1*retval;

	return retval;
}

uint32_t h3(uint32_t a,uint32_t b){
	
	return a+b;
}



