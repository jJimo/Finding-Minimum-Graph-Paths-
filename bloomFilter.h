#ifndef BLOOM
#define BLOOM

	#include <stdio.h>
	#include <stdint.h>
	#include <inttypes.h>
	#include <stdlib.h>

	#define BF_MAYBE 200
	#define BF_NO -200
	#define  BF_SIZE 15482531
	
	char * createBloomFilter(void);

	void removeBloomFilter(char * bf);

	int bloomSet(char * bf,uint32_t a,uint32_t b);

	int bloomCheck(char * bf,uint32_t a,uint32_t b);

	int h1(uint32_t a,uint32_t b);
	uint32_t h2(uint32_t a,uint32_t b);

	uint32_t h3(uint32_t a,uint32_t b);

#endif
