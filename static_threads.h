#ifndef Thre
#define Thre

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
#include "grail.h"

#define perror2(s,e) fprintf(stderr,"%s: %s\n",s,strerror(e))


#define SEQSIZE 10000
#define RES_SIZE 30000000


// Thread Functions
void *worker(void *);

///QUEUE OF jobscheduler
typedef struct  Args{
	uint32_t from;
	uint32_t to;
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
	pthread_mutex_t mutex_start ; 	
	pthread_mutex_t mutex_workersfree; 	//protect variable worker free
	uint32_t *start_from_here;
	int workers_free;

	pthread_cond_t cond_JS; 	
	pthread_cond_t cond_Workers;  

}JobScheduler;

typedef struct global_vars{
	SCC *scc;
	Component **hash;
	uint32_t graph_size;
	NodeIndex *incoming_index,*outcoming_index;
	MetaDataBuffer *b_in,*b_out;
	FILE *fd,*fp;
	//arrays of arrays because each worker has its one dirty bit arrays
	DirtyByteArray **dirty_in;
	DirtyByteArray **dirty_out;
	neighbors **neighbors_forward;
	neighbors **neighbors_backward;	
}global_vars;

JobScheduler *initialize_scheduler(int thread_num);		//create job scheduler
int destroy_scheduler(void);  //destroy job scheduler
int submit_jobs(void);  
void execute_all_jobs(void);
void wait_all_task_finish(void); //waits all submitted task to finish

int questionJob(Args * args, int t_id);	 //execute the job using grail bbfs  etc 
void workers_exit(void);
void execute_print(void);
void wait_print(void);
void destroyStructs(void);

extern global_vars gv;
extern JobScheduler *js;


#endif



