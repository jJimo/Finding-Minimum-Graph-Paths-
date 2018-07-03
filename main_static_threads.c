#include <pthread.h>
#include "static_threads.h"


global_vars gv;
JobScheduler *js;
int M;

int main(int argc,char* argv[]){
	char c;
	int err, i;
	edge ed;


	if(argc < 5){						//1.graph call, 2.Graph, 3.Workload 4.Number Of Threads 5.M:numofjobs per thread execution 
		printf("Correct syntax is: %s <input_file> <workload_file> <threads_number> <thread_jobs_per_time>\n", argv[0]);
		return -1;
	}

	gv.incoming_index = createNodeIndex();
	gv.b_in = createBuffer();

	gv.outcoming_index = createNodeIndex();
	gv.b_out = createBuffer();

	err = 0;
	//-----------------------------------READ GRAPH-----------------------------------//
	gv.fp = fopen(argv[1],"r");
	gv.fd = fopen(argv[2],"r");	
	
	
	if((gv.fp == NULL) || (gv.fd == NULL)) {									//check if fopen failed
		printf("Error:Open files from command line\n");
		return -1;
	}

	while (fscanf(gv.fp,"%c",&c),c!='S') {									//while i do not read character S
			fseek(gv.fp,-sizeof(char),SEEK_CUR);		//go back by sizeof(char) bytes in the file,in order to not losing any bytes of numbers
			fscanf(gv.fp," %"SCNd32" %"SCNd32" ",&(ed.start),&(ed.end));	//scan from file an edge struct composed of two uint32_t numbers separated by space
		
			//then we perform the insert to the buffer 
			//a)insert edge start->end to outcoming_index
			err=insertNode(gv.outcoming_index,gv.b_out,ed.start,ed.end, 0);
			if(err == FAIL) {	
				printf("ERROR:InsertNode - outgoing\n");
				return FAIL;
			}

			//b)insert start->end to incoming_index
			err=insertNode(gv.incoming_index,gv.b_in,ed.end,ed.start, 0);
			if(err == FAIL) {	
				printf("ERROR:InsertNode - incoming\n");
				return FAIL;
			}

	}
	printf("End of input reached\n");
	
	
	
	for(i = 0; i< (MAX((gv.incoming_index->size) , (gv.outcoming_index->size))) ; i++){
		if(((gv.outcoming_index->size > i) && (gv.outcoming_index->index[i] != -1)) || ((gv.incoming_index->size > i)  && (gv.incoming_index->index[i] != -1)))
			continue;
		break;
	}
	
	gv.graph_size = i;
	
	gv.scc = estimateStronglyConnectedComponents(gv.outcoming_index,gv.b_out,gv.graph_size);
	//PrintAllComponents(scc->components);
	constructHyperGraph(&(gv.scc), gv.outcoming_index, gv.b_out);
	buildGrailIndex(gv.scc);
	gv.hash=createComponentsArray(gv.scc);
	
	
	/////////////- threads_code -/////////////
	
	M = atoi(argv[4]);
	js = initialize_scheduler(atoi(argv[3]));		
	
	gv.dirty_in = malloc((js->threadsnum) *sizeof(char *));
	gv.dirty_out = malloc((js->threadsnum) *sizeof(char *));
	gv.neighbors_forward = malloc((js->threadsnum) *sizeof(neighbors *));
	gv.neighbors_backward = malloc((js->threadsnum) *sizeof(neighbors *));;
	for (i=0;i<(js->threadsnum);i++){
		gv.neighbors_forward[i] = create_neighbor_array();
		gv.neighbors_backward[i] = create_neighbor_array();
		gv.dirty_in[i] = create_dirtybyte_array(gv.graph_size);
		gv.dirty_out[i] = create_dirtybyte_array(gv.graph_size);
	}
	
	for(i=0;i<(js->threadsnum);i++){
		int *arg = malloc(sizeof(int));//---------------------------------------------------------------------------------------------?????????
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
		//wait_print();
		execute_all_jobs();
		wait_all_task_finish(); 
		
		//print results!
		execute_print();	
		
	}
	
	workers_exit();
	
	for(i=0;i<(js->threadsnum);i++)
		pthread_join(js->tids[i],NULL);
		
	
	
	printf("End of workload reached\n");
	
	destroyStructs();
	destroy_scheduler(); 
}


