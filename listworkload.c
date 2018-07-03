#include "index.h"
#include "listworkload.h"
uint_list *wl_list_last = NULL;

void insert_uint(uint_list **l,uint32_t st, uint32_t e, char *act) {
	if(*l == NULL){
		(*l) = malloc(sizeof(uint_list));
		(*l)->start = st;
		(*l)->end = e;
		strcpy((*l)->action, act);
		(*l)->next = NULL;
		wl_list_last = *l;
	}
	else {
		uint_list *new = wl_list_last->next;
		new = malloc(sizeof(uint_list));
		new->start = st;
		new->end = e;
		strcpy(new->action, act);
		new->next = NULL;
		wl_list_last->next = new;
		wl_list_last = new;
		//insert_uint(&(*l)->next, st,e,act);

	}
}

void print_list_workload(uint_list *l) {

	while(l!=NULL){
		printf("%s %"SCNd32" %"SCNd32" \n",l->action , l->start, l->end);
		l = l->next;
	}
}

void free_list(uint_list **l) {
	uint_list *p;

	while(*l != NULL){
		p = *l;
		*l = (*l)->next;
		free(p);
	}
}



