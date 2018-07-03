#ifndef DynamicThr
#define DynamicThr


#include <stdio.h>
#include <string.h>    
#include <stdlib.h>   
#include <time.h> 
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <unistd.h>    
#include <pthread.h>
#include <signal.h>  
#include <stdint.h>

#include "byte_array_handler.h"
#include "connectedComponents.h"
#include "listworkload.h"
#include "bloomFilter.h"


#define perror2(s,e) fprintf(stderr,"%s: %s\n",s,strerror(e))

#define REBUILD 2
#define SEQSIZE 10000
#define RES_SIZE 30000000


typedef struct {
	uint32_t start;
	uint32_t end;
}edge;


// Thread Functions
void *worker(void *);


///QUEUE OF jobscheduler
typedef struct  Args{
	uint32_t from;
	uint32_t to;
	uint32_t version;
}Args;

typedef struct Job{
	int (*j)(Args *,int);
	Args *args;
}Job;

typedef struct JobScheduler{
	int threadsnum; 	//how many threads we got
	Job *q;				//the queue of jobs
	uint32_t head,tail;  //start and end of queue (remember ,insert at end,delete from start)
	int q_size;			//size of queue.
	int *results;	// result array
	int num_of_res;
	pthread_t *tids;	 // the array of thread ids

	//and now mutexes-condition variables for synch
	pthread_mutex_t mutex_start ; 	// protects array start_From_here, which says to everyone where to take job
	pthread_mutex_t mutex_workersfree; 	//protect variable worker free
	uint32_t *start_from_here;
	int workers_free;
	
	pthread_cond_t cond_JS; 	// wait  at that condition varibale until any worker becomes availble 
	pthread_cond_t cond_Workers;  //  workers wait on that condition variable until they get work

	pthread_mutex_t queriesUpdateIndex;

}JobScheduler;

typedef struct global_vars_Dynamic{
	
	NodeIndex *incoming_index,*outcoming_index;

	MetaDataBuffer *b_in,*b_out;
	FILE *fp,*fd ;
	
	CC * cc;

	DirtyByteArray ** dirty_incoming;
	DirtyByteArray ** dirty_outcoming;
	int old_incoming_size;
	int old_outcoming_size;
	neighbors ** forward_neighbors;
	neighbors ** backward_neighbors;

	char * bf;
	char * line;
	
	uint32_t current_version;
	
}global_vars_Dynamic;


JobScheduler *initialize_scheduler(int thread_num);		//create job scheduler
int destroy_scheduler(void);  //destroy job scheduler
int submit_jobs(void);  //gives M jobs at each thread.
void execute_all_jobs(void);
void wait_all_task_finish(void); //waits all submitted task to finish

int questionJob(Args * args, int t_id);	 //execute the job using grail bbfs  etc 
void workers_exit(void);
void execute_print(void);
void wait_print(void);

extern global_vars_Dynamic gvD;
extern JobScheduler *js;


int initStructs(char* argv[],edge * ed);

void destroyStructs(void);


#endif
