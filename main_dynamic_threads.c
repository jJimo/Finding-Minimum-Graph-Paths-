#include <stdio.h>
#include <string.h>    
#include <stdlib.h>   
#include <time.h> 
#include <ctype.h>
#include <unistd.h>    
#include <pthread.h>
#include <signal.h>  
#include <stdint.h>

#include "listworkload.h"
#include "connectedComponents.h"
#include "bfs.h"
#include "buffer.h"
#include "dynamic_threads.h"

global_vars_Dynamic gvD;
JobScheduler *js;
int M;


int main(int argc,char* argv[]) {
	int err,i;
	edge ed;

	if(argc < 5){						//1.graph call, 2.Graph, 3.Workload 4.Number Of Threads 5.M:numofjobs per thread execution 
		printf("Correct syntax is: %s <input_file> <workload_file> <threads_number> <thread_jobs_per_time>\n", argv[0]);
		return -1;
	}
	
	M = atoi(argv[4]);
	js = initialize_scheduler(atoi(argv[3]));
	

	err=initStructs(argv,&ed);
	if(err<0)
		return -1;
		
		
	/////////////////////////////////////////////////////////////////
	
	for(i=0;i<(js->threadsnum);i++){
		int *arg = malloc(sizeof(int));
        if ( arg == NULL ) {
            fprintf(stderr, "Couldn't allocate memory for thread arg.\n");
            exit(EXIT_FAILURE);
        }

        *arg = i;
		if( pthread_create( &(js->tids[i]) , NULL ,  worker , arg) < 0) {  //create workers
			printf("could not create thread\n");
			return -1;
		}	
	}

	int exit;
	while((exit=submit_jobs()) == 1){ 
		execute_all_jobs();
		wait_all_task_finish(); 
		
		//print results!
		execute_print();	
		
	}
	
	workers_exit();
	
	for(i=0;i<(js->threadsnum);i++)
		pthread_join(js->tids[i],NULL);
		

	printf("End of workload reached\n");
	
	
	destroyStructs();//destroy all structs.
	destroy_scheduler(); 
	
	return 0;
}
