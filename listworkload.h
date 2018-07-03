#ifndef listworkload
#define listworkload

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


typedef struct uint_list{
	char action[10];
	uint32_t start;
	uint32_t end;
	struct uint_list *next;
}uint_list;

void insert_uint(uint_list **,uint32_t,uint32_t,char*);
void print_list_workload(uint_list *);
void free_list(uint_list **);

#endif
