#include "dynamic_threads.h"
#include "listworkload.h"
#include "connectedComponents.h"
#include "bfs.h"
#include "buffer.h"
#include "dynamic_threads.h"


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
	pthread_mutex_init(&(js->queriesUpdateIndex), NULL);
	


	
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
	if (err = pthread_mutex_destroy(&js->queriesUpdateIndex)) { /* Destroy mutex */
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

int submit_jobs(void){  //dinei M jobs se kathe thread.
	gvD.line = NULL;
	uint32_t i,err, path;
	int status;
    size_t len = 0;
    ssize_t read;
	char service[100];
	edge ed;
	pid_t pid;
	static char prev='X';
	uint32_t start,end;

	while ((read = getline(&(gvD.line), &len, gvD.fd)) != -1) {													//Read Workload File line by line
		sscanf((gvD.line),"%s%"SCNd32"%"SCNd32"",service,&(ed.start),&(ed.end));
																		//run all commands of l
		if (strcmp(service,"A")==0){				//if command is A
			
			if(prev=='Q')
				gvD.current_version++;
			
			prev='A';
			
			gvD.old_outcoming_size=gvD.outcoming_index->size;
			gvD.old_incoming_size=gvD.incoming_index->size;
			
			if(bloomCheck(gvD.bf,ed.start,ed.end)==BF_MAYBE){

				if( search(gvD.outcoming_index,gvD.b_out,ed.start,ed.end) == FAIL ) {					//check if edge is already exists
					bloomSet(gvD.bf,ed.start,ed.end);
					insertNode(gvD.outcoming_index,gvD.b_out,ed.start,ed.end, gvD.current_version);						//if edge not exists, insere new edge to outcoming 
					if(err == FAIL) {	
						printf("ERROR:InsertNode - outgoing\n");
						return FAIL;
					}
					
					insertNode(gvD.incoming_index,gvD.b_in,ed.end,ed.start, gvD.current_version);							//and incoming
					if(err == FAIL) {	
						printf("ERROR:InsertNode - incoming\n");
						return FAIL;
					}

					if (gvD.old_outcoming_size!=gvD.outcoming_index->size){
						for (i=0;i<(js->threadsnum);i++){
							int err = realloc_dirtybyte_array(gvD.dirty_outcoming[i],gvD.outcoming_index->size);
							if(err<0){
								perror("ERROR: sto realloc tou dirtybyte\n");
								return -1;
							}
						}
					}
					
					if (gvD.old_incoming_size!=gvD.incoming_index->size){
						for (i=0;i<(js->threadsnum);i++){
							int retval = realloc_dirtybyte_array(gvD.dirty_incoming[i],gvD.incoming_index->size);
							if(retval<0){
								perror("ERROR: sto reallocate tou DirtyByteArray\n");
								return retval;
							}
						}
					}
				
					
					int ok = insertNewEdgeToCC(gvD.cc, ed.start, ed.end);	
					if(ok != OK_SUCCESS){
						printf("Error: insertNewEdgeToCC..   (action==\"A\") \n");
						return ok;
					}
					
				}
			}
			else{
					bloomSet(gvD.bf,ed.start,ed.end);
					insertNode(gvD.outcoming_index,gvD.b_out,ed.start,ed.end, gvD.current_version);						//if edge not exists, insere new edge to outcoming 
					if(err == FAIL) {	
						printf("ERROR:InsertNode - outgoing\n");
						return FAIL;
					}
					
					insertNode(gvD.incoming_index,gvD.b_in,ed.end,ed.start, gvD.current_version);							//and incoming
					if(err == FAIL) {	
						printf("ERROR:InsertNode - incoming\n");
						return FAIL;
					}

					if (gvD.old_outcoming_size!=gvD.outcoming_index->size){
						for (i=0;i<(js->threadsnum);i++){
							int err = realloc_dirtybyte_array(gvD.dirty_outcoming[i],gvD.outcoming_index->size);
							if(err<0){
								perror("ERROR: sto realloc tou dirtybyte\n");
								return -1;
							}
						}
					}
					
					if (gvD.old_incoming_size!=gvD.incoming_index->size){
						for (i=0;i<(js->threadsnum);i++){
							int retval = realloc_dirtybyte_array(gvD.dirty_incoming[i],gvD.incoming_index->size);
							if(retval<0){
								perror("ERROR: sto reallocate tou DirtyByteArray\n");
								return retval;
							}
						}
					}
				
					
					int ok = insertNewEdgeToCC(gvD.cc, ed.start, ed.end);	
					if(ok != OK_SUCCESS){
						printf("Error: insertNewEdgeToCC..   (action==\"A\") \n");
						return ok;
					}

			}
		}
		else if (strcmp(service,"Q")==0) {				//if  command is Q
			
			(gvD.cc->all_qs)++;
			prev='Q';
			
			if(js->tail >= js->q_size){
				js->q = realloc(js->q ,2 * (js->q_size)* sizeof(Job));
				for(i=js->q_size; i < 2 * (js->q_size);i++)
					js->q[i].args = malloc(sizeof(Args));
				(js->q_size) = 2*(js->q_size);
			}		

			js->q[js->tail].j = &questionJob;
			js->q[js->tail].args->from = ed.start;
			js->q[js->tail].args->to = ed.end;
			js->q[js->tail].args->version = gvD.current_version;  //set current version for this query.
			
			(js->tail)++;
																//print shortest path from "start" to "end" 
		}
		else if (strcmp(service,"F")== 0){

			////- rebuildIndexes if is necessary -////
			if(gvD.cc->all_qs!=0){
				gvD.cc->metricVal = gvD.cc->q_that_used_ui/gvD.cc->all_qs;
				float reb = REBUILD;

				if(gvD.cc->metricVal>reb){

					if(rebuildIndexes(&(gvD.cc)) != OK_SUCCESS){
						printf("ERROR:rebuild\n");
						return -2;
					}
					
					
					gvD.cc->all_qs=0;
					pthread_mutex_lock(&(js->queriesUpdateIndex));
					gvD.cc->q_that_used_ui = 0;
					pthread_mutex_unlock(&(js->queriesUpdateIndex));
				}
			}
			////- end_rebuildIndexes if is necessary_end -////
			free(gvD.line);
			(js->num_of_res)=(js->tail);
			return 1;  //end of curent batch
		}
		
	}
	free(gvD.line);
	(js->num_of_res)=(js->tail);
	return 0;  //end of file
}

void execute_all_jobs(void){
	int i;
	while(js->tail>js->head){//while until we eat the queue
		pthread_mutex_lock(&(js->mutex_start));
		for(i=0;i<(js->threadsnum);i++){
			if(js->start_from_here[i]==-1){//if thread is available(does not sleep)
				if((js->head)<(js->tail)){
					js->start_from_here[i]=js->head;//give it work
					(js->head)+=M;
				}
				else{
					js->start_from_here[i]=-1;//if queue has ended i do not give work
				}
			}
		}
		pthread_cond_broadcast(&(js->cond_Workers));
		pthread_mutex_unlock(&(js->mutex_start));
		
		pthread_mutex_lock(&(js->mutex_workersfree));
			
		while(js->workers_free==0)//-!avoid busy waiting...
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
	
	//initialization of queue's tail and head
	js->tail=0;
	js->head=0;
}

int questionJob(Args * args, int t_id){	//function that executes the job
	int u;
	int do_bfs= -1;
	int err;
	uint32_t  ans,start,end;
	uint32_t path=-1;
	
	if( (args->from) == (args->to) )//An yparxei erotima apo kai pros ton idio kombo epestrepse 0.
		return 0;


	//if indexed did not increase
	start = findNodeConnectedComponentID(gvD.cc, args->from);
	end = findNodeConnectedComponentID(gvD.cc, args->to);
	
	if( (start<0)  || (end<0) ){
		perror("Erotima(Q) me konbo pou dn iparxei stin domi anixneftike!!\n");
		return -1;
	}
	
	if( start != end ){
		pthread_mutex_lock(&(js->queriesUpdateIndex));
		(gvD.cc->q_that_used_ui)++;
		pthread_mutex_unlock(&(js->queriesUpdateIndex));
		
		int retval = findPairIntoUpdateIndex(gvD.cc,start,end); //search if connection between to cc exist
		if( retval == NOT_EXIST ){
			path = -1;
			do_bfs=0;
		}
		else if( retval == EXIST ){
			do_bfs=1;
		}
		else{
			perror("Error: findPairIntoUpdateIndex called from main_dynamic\n");
			return retval;
		}
	}
	else{
		do_bfs=1;
	}


	///////////////////////////////-- do bfs if required --/////////////////////////////////
	if(do_bfs==1){	
		
		path = bfs(gvD.incoming_index,gvD.b_in,gvD.outcoming_index,gvD.b_out,args->from,args->to,args->version,gvD.dirty_incoming[t_id],gvD.dirty_outcoming[t_id],gvD.forward_neighbors[t_id],gvD.backward_neighbors[t_id]);		//Find shortest path
		if(path == -2) {
			printf("ERROR:BFS\n");
			return -1;
		}

	}

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

///////////////////////////////////////////////////////////////////////////////////////////////

int initStructs(char* argv[],edge * ed){
	char c;
	int err = 0;
	int i;
	
	gvD.bf = createBloomFilter();
	
	gvD.current_version=0;
	
	gvD.incoming_index = createNodeIndex();
	gvD.b_in = createBuffer();

	gvD.outcoming_index = createNodeIndex();
	gvD.b_out = createBuffer();

	gvD.fp = fopen(argv[1],"r");
	gvD.fd = fopen(argv[2],"r");

	/*if ((pid = fork()) == 0) {
			execl("./script.sh"," script.sh", argv[1], NULL);											//run sctipt that remove duplicates from input_graph
			printf("ERROR EXCECL\n");
			return -1;
	}
	
	waitpid(pid, &status, 0);																			//wait for process pid to finish
	*/

	if((gvD.fp == NULL) || (gvD.fd == NULL)) {																	//check if fopen failed
		printf("Error:Open files from command line\n");
		return -1;
	}

	while (fscanf(gvD.fp,"%c",&c),c!='S') {																	//while i do not read character S
			fseek(gvD.fp,-sizeof(char),SEEK_CUR);															//go back by sizeof(char) bytes in the file,in order to not losing any bytes of numbers
			fscanf(gvD.fp," %"SCNd32" %"SCNd32" ",&(ed->start),&(ed->end));									//scan from file an edge struct composed of two uint32_t numbers separated by space
		
			//then we perform the insert to the buffer 
			//a)insert edge start->end to outcoming_index
			err=insertNode(gvD.outcoming_index,gvD.b_out,ed->start,ed->end,0);  
			if(err == FAIL) {	
				printf("ERROR:InsertNode - outgoing\n");
				return FAIL;
			}

			//b)insert start->end to incoming_index
			err=insertNode(gvD.incoming_index,gvD.b_in,ed->end,ed->start,0);
			if(err == FAIL) {	
				printf("ERROR:InsertNode - incoming\n");
				return FAIL;
			}

	}
	printf("End of input reached\n");

	gvD.cc = estimateConnectedComponents(gvD.outcoming_index,gvD.incoming_index, gvD.b_out, gvD.b_in);

	gvD.dirty_incoming = malloc((js->threadsnum) * sizeof(DirtyByteArray *));
	gvD.dirty_outcoming = malloc((js->threadsnum) * sizeof(DirtyByteArray *));
	gvD.forward_neighbors = malloc((js->threadsnum) * sizeof(neighbors *));
	gvD.backward_neighbors = malloc((js->threadsnum) * sizeof(neighbors *));

	for (i=0;i<(js->threadsnum);i++){
		gvD.forward_neighbors[i] = create_neighbor_array();
		gvD.backward_neighbors[i] = create_neighbor_array();
		gvD.dirty_incoming[i] = create_dirtybyte_array(gvD.incoming_index->size);
		gvD.dirty_outcoming[i] = create_dirtybyte_array(gvD.outcoming_index->size);
	}

}

void destroyStructs(void){
	int i;
	//Destruction of our structures
	for (i=0;i<(js->threadsnum);i++){
		free(gvD.forward_neighbors[i]);
		free(gvD.backward_neighbors[i]);
		free(gvD.dirty_incoming[i]);
		free(gvD.dirty_outcoming[i]);
	}
	free(gvD.dirty_incoming);
	free(gvD.dirty_outcoming);
	free(gvD.forward_neighbors);
	free(gvD.backward_neighbors);


	fclose(gvD.fd);
	fclose(gvD.fp);
	destroyNodeIndex(gvD.outcoming_index);
	destroyNodeIndex(gvD.incoming_index);
	destroyBuffer(gvD.b_in);
	destroyBuffer(gvD.b_out);	
	
	destroyConnectedComponents(gvD.cc);
	
	removeBloomFilter(gvD.bf);
	
}



