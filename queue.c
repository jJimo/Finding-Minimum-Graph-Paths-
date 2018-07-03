#include "threads.h"


void enqueue(struct queue *q, Job j, uint32_t from, uint32_t to) {
	struct node *p;

	p = malloc(sizeof(struct node));
	p->job = j;
	p->job.from = from;
	p->job.to = to;
	p->next = NULL;
	if (q->tail == NULL)
		q->head = p;
	else
		q->tail->next = p;
	q->tail = p;
}

int dequeue(struct queue *q,Job *j, uint32_t *from, uint32_t *to) {
	struct node *p;

	p = q->head;
	if (q->head == NULL)
		return -1;
	else {
		*j = q->head->job;
		*from = q->head->job.from;
		*to = q->head->job.to;
		q->head = q->head->next;
		free(p);
		if (q->head == NULL)
			q->tail = NULL;
		return 0;
	}
}

void printqueue (struct queue q) {
	struct node *h;

	h = q.head;
	while (h != NULL) {
		printf ("%d %d\n", h->job.from, h->job.to);
		h = h->next;
	}
}
