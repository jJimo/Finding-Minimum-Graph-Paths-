#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[]){
	FILE *fd;
	char first_word[15];




	if(argc < 5){														//1.graph call, 2.Graph, 3.Workload 4.Number Of Threads 5.M:numofjobs per thread execution 
		printf("Correct syntax is: %s <input_file> <workload_file> <threads_number> <thread_jobs_per_time>\n", argv[0]);
		return -1;
	}
	//open the file of the second arg.if the first line says static then go for static_graph with same arguments
	//else dynamic_graph with the same arguments
	fd=fopen(argv[2],"r");
	fscanf(fd,"%s",first_word);
	fclose(fd);

	if (strcmp(first_word,"STATIC")==0){
		execl("./graph_static","graph_static", argv[1],argv[2],argv[3],argv[4], NULL);					
		printf("ERROR EXCECL\n");
		return -1;
	}
	else if (strcmp(first_word,"DYNAMIC")==0){
		execl("./graph_dynamic","graph_dynamic", argv[1],argv[2],argv[3],argv[4],NULL);											
		printf("ERROR EXCECL\n");
		return -1;
	}

	return 1;
}