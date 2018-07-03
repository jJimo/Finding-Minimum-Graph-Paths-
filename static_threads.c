#include "static_threads.h"

extern int M;

JobScheduler *initialize_scheduler(int thread_num){
	int i;
	
	JobScheduler * js = malloc(sizeof(JobScheduler));
	js->threadsnum = thread_num;
	js->tids = malloc(thread_num * sizeof(pthread_t));
	js->start_from_here = malloc(thread_num * sizeof(uint32_t));
	//init personal vars
	for(i=0;i<thread_num;i++){
		js->start_from_here[i]= -1 ;	//in order to worker waiting
	}
	js->workers_free=thread_num;
	
	(js->q) = malloc(SEQSIZE * sizeof(Job));


	for(i=0; i<SEQSIZE;i++)
		js->q[i].args = malloc(sizeof(Args));

	js->tail = 0;
	js->head = 0;
	js->q_size=SEQSIZE;
	
	js->results=malloc(RES_SIZE * sizeof(int));
	js->num_of_res=0;
	//init Condition Variables
	pthread_cond_init(&(js->cond_JS), NULL);
	pthread_cond_init(&(js->cond_Workers), NULL);

	
	//init mutexes
	pthread_mutex_init(&(js->mutex_start), NULL);
	pthread_mutex_init(&(js->mutex_workersfree), NULL);


	
	return js;
}


int destroy_scheduler(void){
	int err;
	if (err = pthread_mutex_destroy(&js->mutex_workersfree)) { /* Destroy mutex */
	 	perror2("pthread_mutex_destroy ", err); exit(1);
	}
	if (err = pthread_mutex_destroy(&js->mutex_start)) { /* Destroy mutex */
	 	perror2("pthread_mutex_destroy ", err); exit(1);
	}
	if (err = pthread_cond_destroy(&js->cond_JS)) { /* Destroy condvar */
	 	perror2("pthread_condvar_destroy ", err); exit(1);
	}
	if (err = pthread_cond_destroy(&js->cond_Workers)) { /* Destroy condvar */
	 	perror2("pthread_condvar_destroy ", err); exit(1);
	}	
	int i;
	for(i=0; i<SEQSIZE;i++)
		free(js->q[i].args);
	free(js->q);

	free(js->start_from_here);
	free(js->tids);
	free(js->results);
	free(js);
}


int submit_jobs(void){  //gives M jobs at every thread.
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char service[100];
	edge ed;
    int i;
    while ((read = getline(&line, &len, gv.fd)) != -1) {
		sscanf(line,"%s%"SCNd32"%"SCNd32"",service,&(ed.start),&(ed.end));
		if (strcmp(service,"Q")== 0){	
			if(js->tail >= js->q_size){
				js->q = realloc(js->q ,2 * (js->q_size)* sizeof(Job));
				for(i=js->q_size; i < 2 * (js->q_size);i++)
					js->q[i].args = malloc(sizeof(Args));
				(js->q_size) = 2*(js->q_size);
			}		

			js->q[js->tail].j = &questionJob;
			js->q[js->tail].args->from = ed.start;
			js->q[js->tail].args->to = ed.end;
			(js->tail)++;	

		}
		else if (strcmp(service,"F")== 0){
			(js->num_of_res)=(js->tail);
			return 1;  //end of curent batch
		}
	}
	
	(js->num_of_res)=(js->tail);
	return 0;  //end of file
	
}

void execute_all_jobs(void){
	int i;
	while(js->tail>js->head){//until queue ends
		pthread_mutex_lock(&(js->mutex_start));
		for(i=0;i<(js->threadsnum);i++){
			if(js->start_from_here[i]==-1){//if thread is availablee)
				if((js->head)<(js->tail)){
					js->start_from_here[i]=js->head;//give work
					(js->head)+=M;
				}
				else{
					js->start_from_here[i]=-1;//if q is empty i do not give job.
				}
			}
		}
		pthread_cond_broadcast(&(js->cond_Workers));
		pthread_mutex_unlock(&(js->mutex_start));
		
		pthread_mutex_lock(&(js->mutex_workersfree));
			
		while(js->workers_free==0)//-!avodi busy waiting...
			pthread_cond_wait(&(js->cond_JS),&(js->mutex_workersfree));
			
		pthread_mutex_unlock(&(js->mutex_workersfree));
		
		
	}
}
	



void wait_all_task_finish(void){ //waits all submitted task to finish
	
	int flag=0;
	pthread_mutex_lock(&(js->mutex_start));
	int i;
	for(i=0;i<js->threadsnum;i++){
		if(js->start_from_here[i]!=-1){
			flag=1;
			break;	
		}
	}
	while(flag==1){
		pthread_cond_wait(&(js->cond_JS),&(js->mutex_start));
		flag=0;
		for(i=0;i<(js->threadsnum);i++){
			if(js->start_from_here[i]!=-1){
				flag=1;
				break;	
			}
		}
	}
	pthread_mutex_unlock(&(js->mutex_start));
	
	js->tail=0;
	js->head=0;
}


int questionJob(Args * args, int t_id){	//function that executes the job
	int err;
	uint32_t  ans;
	uint32_t path;
	
	if( (args->from) == (args->to) )//If query with same to-from node return 0.
		return 0;
	
	ans=isReachableGrailIndex(gv.scc,gv.hash,args->from,args->to);
	if (ans == YES){ // we search for a path at the same component
		path = bfs_SCC(gv.scc,gv.incoming_index,gv.b_in,gv.outcoming_index,gv.b_out,args->from,args->to,gv.dirty_in[t_id]->array,gv.dirty_out[t_id]->array,gv.neighbors_forward[t_id],gv.neighbors_backward[t_id]);		//Find shortest path
		if(path == -2) {
			printf("ERROR:BFS\n");				
		}
	}
	
	else if (ans == NO)
		path = -1;
	else if (ans == MAYBE)		// then we call bfs from the first assignment
		path = bfs_GRAIL(gv.incoming_index,gv.b_in,gv.outcoming_index,gv.b_out,args->from,args->to,gv.dirty_in[t_id]->array,gv.dirty_out[t_id]->array,gv.neighbors_forward[t_id],gv.neighbors_backward[t_id],gv.scc,gv.hash);
		//path = bfs(incoming_index,b_in,outcoming_index,b_out,temp->start,temp->end,dirty_incoming,dirty_outcoming,forward_neighbors,backward_neighbors);

	return path;

}

void *worker(void *argp){
	int i,path;
	int th_id = *((int *) argp);  
	while(1){
		
		pthread_mutex_lock(&(js->mutex_start));

		while(js->start_from_here[th_id]==-1)
			pthread_cond_wait(&(js->cond_Workers),&(js->mutex_start));
		
		if(js->start_from_here[th_id]==-2){
			pthread_mutex_unlock(&(js->mutex_start));
			break;
			
		}
			
		pthread_mutex_unlock(&(js->mutex_start));

		pthread_mutex_lock(&(js->mutex_workersfree));
		(js->workers_free)--;
		pthread_mutex_unlock(&(js->mutex_workersfree));
		//-------------exe_Job------------------//
		i=js->start_from_here[th_id];
		while(( i < (js->start_from_here[th_id]+M) ) && ( i < (js->tail) )){
			
			path=(js->q[i].j)(js->q[i].args, th_id); //call the corresponding job function for each job.
			js->results[i]=path;
			
			i++;
		}
		//-------------exe_Job------------------//
		pthread_mutex_lock(&(js->mutex_workersfree));
		
		pthread_mutex_lock(&(js->mutex_start));
		js->start_from_here[th_id]=-1;
		pthread_mutex_unlock(&(js->mutex_start));
		
		(js->workers_free)++;
		pthread_cond_signal(&(js->cond_JS));//kalw ta threads na doulepsoune
		
		pthread_mutex_unlock(&(js->mutex_workersfree));

	}
	pthread_exit(NULL);
	free(argp);
}

void workers_exit(void){
	int i;
	pthread_mutex_lock(&(js->mutex_start));
	for(i=0;i<(js->threadsnum);i++)
		js->start_from_here[i]=-2;
	
	pthread_cond_broadcast(&(js->cond_Workers));
	pthread_mutex_unlock(&(js->mutex_start));
}


void execute_print(void){
	
	int i;
	for(i=0;i<(js->num_of_res);i++){
			printf("%d\n",js->results[i]);
	}
	
}


void wait_print(void){
	
	
	
}


void destroyStructs(void){
	int i;
	//Destruction of our structures

	for (i=0;i<(js->threadsnum);i++){
		free(gv.neighbors_forward[i]);
		free(gv.neighbors_backward[i]);
		free(gv.dirty_in[i]);
		free(gv.dirty_out[i]);
	}
	
	free(gv.dirty_in);
	free(gv.dirty_out);
	free(gv.neighbors_forward);
	free(gv.neighbors_backward);

	destroyStronglyConnectedComponents(&gv.scc);;
	
	fclose(gv.fd);
	fclose(gv.fp);
	destroyNodeIndex(gv.outcoming_index);
	destroyNodeIndex(gv.incoming_index);
	destroyBuffer(gv.b_in);
	destroyBuffer(gv.b_out);

	
}


