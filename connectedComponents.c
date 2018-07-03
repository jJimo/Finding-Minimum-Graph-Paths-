#include "connectedComponents.h"
#include "dynamic_threads.h"

//This function inserts a new recording(the connected components ID that each node has) into ccindex
//In case of space shortage the ccindex will reallocated.
int insertIntoCCIndex(CC * cc, uint32_t nodeId, uint32_t ccNumId){
	if( (nodeId < (cc->size_of_ccindex)) && (nodeId >= 0) ){ 	 //If there is empty space...
		cc->ccindex[nodeId] = ccNumId;
		(cc->ccindexNumOfRecs) = (cc->ccindexNumOfRecs) + 1;
	}
	else if(nodeId >= 0){			//If address does not exist reallocate the array
		uint32_t * temp_pointer = cc->ccindex ;
		
		if( nodeId > ((cc->size_of_ccindex)*2) ){
			temp_pointer = realloc(cc->ccindex, sizeof(uint32_t) * (nodeId + 1)); //pigene ton realloc apefthias sto (nodeIdS + 1).
			if(temp_pointer == NULL){
				printf("ERROR: realloc ccindex\n");				
				return CCINDEX_REALLOC_ERROR;
			}
			cc->size_of_ccindex = sizeof(uint32_t) * (nodeId + 1) ;
			cc->ccindex = temp_pointer; // Replace of ccindex pointer with new one
			cc->ccindex[nodeId] = ccNumId;
			(cc->ccindexNumOfRecs) = (cc->ccindexNumOfRecs) + 1;
		}
		else{
			temp_pointer = realloc(cc->ccindex, sizeof(uint32_t) * (cc->size_of_ccindex) * 2); // Doubling of ccindex
			if(temp_pointer==NULL){
				return CCINDEX_REALLOC_ERROR;
			}
			else{
				cc->size_of_ccindex = (cc->size_of_ccindex) * 2 ;
				cc->ccindex = temp_pointer; // Replace of ccindex pointer with new one
				cc->ccindex[nodeId]=ccNumId;
				(cc->ccindexNumOfRecs) = (cc->ccindexNumOfRecs) + 1;
			}
		}
	}
	else{
		return OUT_OF_RANGE;
	}

	return OK_SUCCESS;
}

//This function inserts a new edge into update updateIndex.
//In case of space shortage the ccindex will reallocated.
int insertNewEdgeToCC(CC * cc, uint32_t nodeIdS, uint32_t nodeIdE){

	//---Insert new edges into update index---//
	
	uint32_t ccA = -1 , ccB = -1;
	ccA = findNodeConnectedComponentID(cc, nodeIdS);
	ccB = findNodeConnectedComponentID(cc, nodeIdE);
	//printf("{%d,%d}\n",ccA,ccB);
	
	if((ccA >= 0) && (ccB >= 0)){
		int reval = insertNewCCPairIntoUpdateIndex(cc,ccA,ccB);
		if(reval<0)
			return reval;
	}
	else if((ccA <0) && (ccB >=0)){
		
		int reval = insertIntoCCIndex(cc, nodeIdS, ccB);
		if(reval<0)
			return reval;
			
	}
	else if((ccA >=0) && (ccB <0)){
		
		int reval = insertIntoCCIndex(cc, nodeIdE, ccA);
		if(reval<0)
			return reval;
			
	}
	else if((ccA <0) && (ccB <0)){
		
		int reval = insertIntoCCIndex(cc, nodeIdS, (cc->ccCounter));
		if(reval<0)
			return reval;
			
		reval = insertIntoCCIndex(cc, nodeIdE, (cc->ccCounter));
		if(reval<0)
			return reval;
		(cc->ccCounter) = (cc->ccCounter) + 1;  //??? prota afxanw kai meta bazw timi i to aditheto ???			
	}
	else{
		return GENERAL_ERROR;
	}
	
	return OK_SUCCESS;
}

//Returns the Number of connected component that nodeId indicates.
uint32_t findNodeConnectedComponentID(CC * cc, uint32_t nodeId){
	
	if((nodeId < (cc->size_of_ccindex)) && (nodeId >= 0) ){
		if(cc->ccindex[nodeId]>=0)
			return (cc->ccindex[nodeId]);
		else
			return UNINIT;
	}
	else{
		return OUT_OF_RANGE;
	}
}


//destroys updateIndex and calls "estimateConnectedComponents()" again
int rebuildIndexes(CC ** cc){
	
	int reval = destroyConnectedComponents(*cc);
	if(reval<0)
		return reval;
		
	*cc = estimateConnectedComponents(gvD.outcoming_index,gvD.incoming_index, gvD.b_out, gvD.b_in);
	if(*cc==NULL)
		return ESTIMATE_CC_ERROR;
	else
		return OK_SUCCESS;	
	
}

//creates the whole CC struct adn allocates space( "ccindex" & "updateIndex" ) .
CC * createConnectedComponents(uint32_t initSizeOfCCIndex){
	
	CC * ccPtr = malloc(sizeof(CC));
	if(ccPtr == NULL){			//In case of error return "NULL" .
		return NULL;
	}

	ccPtr->size_of_ccindex = initSizeOfCCIndex;
	ccPtr->ccindexNumOfRecs = 0 ;
	ccPtr->ccindex=malloc( sizeof(uint32_t) * initSizeOfCCIndex );	//allocate memory for ccindex
	if(ccPtr->ccindex == NULL){			//In case of error free memory and return "NULL" .
		free(ccPtr);
		return NULL;
	}


	ccPtr->updateIndex = createUpdateIndex(INIT_SIZE_OF_UPDATEINDEX); //allocate memory for update index
	if(ccPtr->updateIndex == NULL){ 		//In case of error free memory and return "NULL" .
		free(ccPtr->ccindex);
		free(ccPtr);
		return NULL;
	}
	
	//--Set initial values to the following variables--//
	ccPtr->ccCounter=1;
	ccPtr->metricVal = 0;
	ccPtr->all_qs = 0;
	ccPtr->q_that_used_ui = 0;
	
	return ccPtr;
	
}


//destroys CC structure and frees space( "ccindex" & "updateIndex" ) .
int destroyConnectedComponents(CC * cc){
	
	int reval = destroyUpdateIndex(cc);
	if(reval < 0)
		return FREE_ERROR;
		
	free(cc->ccindex);

	free(cc);
	 
	return OK_SUCCESS;	
}
							
						///----------------------------- UpdateIndex ------------------------------------------------///


UpdateIndex * createUpdateIndex(int InitSizeOfUpdateIndex){
	uint32_t i;
	UpdateIndex *ui;

	//Malloc of an update index structure
	ui = malloc(sizeof(UpdateIndex));

	ui->ccUpdate = malloc( InitSizeOfUpdateIndex * sizeof(ByteTable*) );
	//Initialization of the two above arrays
	for (i=0; i< InitSizeOfUpdateIndex; i++){
		ui->ccUpdate[i] = NULL;
	}
	//set of the two other variables
	ui->size_of_ccUpdate = InitSizeOfUpdateIndex;
	ui->numOfRecs = 0;

	//printf("Update Index succesfully created\n");
	return ui;
	
}

int destroyUpdateIndex(CC * cc){
	uint32_t i,j;
	uint32_t size_aux;
	UpdateIndex * ui = cc->updateIndex;

	for (i=0; i< ui->size_of_ccUpdate; i++){
		if ( ui->ccUpdate[i] != NULL ){		//if  i-th byte array not null
			size_aux = ui->ccUpdate[i]->size;
			for (j=0; j< size_aux; j++){	//then for all elements except itself,make them NULL ,and after free the array and also make NULL	
				if (i==j)
					continue;
				else if (ui->ccUpdate[i]->ccTable[j] == 1){		//if we have a neighbor we make him NULL on the ccUpdate
					ui->ccUpdate[j] = NULL;
				}

			}

			free(ui->ccUpdate[i]->ccTable);
			free(ui->ccUpdate[i]);		//we free byte array of component i
			ui->ccUpdate[i] = NULL;
		}
	}
	//free the two arrrays of the structure and make it NULL
	free(ui->ccUpdate);
	free(ui);
	cc->updateIndex = NULL;

	return OK_SUCCESS;
}

 //check "update_index" if ccA and ccB there are in update list.
int findPairIntoUpdateIndex(CC * cc,uint32_t ccA,uint32_t ccB){
	UpdateIndex * ui = cc->updateIndex;

	if((ccA >= ui->size_of_ccUpdate) || (ccB >= ui->size_of_ccUpdate)){  //If update index smaller than cc,it means we do not have reconrds
		return NOT_EXIST;
	}
	
	if(ui->ccUpdate[ccA]==NULL){		//If ccUpdate is null in posistion ccA,it means we have no records with it inside.
		return NOT_EXIST;
	}
	
	if( ccB < (ui->ccUpdate[ccA]->size) ){		//If size of neigbor_table of ccA is smaller than ccB's, it means that they are not connected
		if((ui->ccUpdate[ccA]->ccTable[ccB]) == 1)
			return EXIST ;
		else if ((ui->ccUpdate[ccA]->ccTable[ccB]) == 0)
			return NOT_EXIST ;
		else
			return UPDATE_UNITIALISE;
	}
	else{   	//If ccb larger than ccA's neigbor table, it means that does not exist any connection.
		return NOT_EXIST;
	}
		
}


//cmd is 0, in order to import neighbors(merge arrays)
int insertNewCCPairIntoUpdateIndex(CC * cc, uint32_t ccA, uint32_t ccB){
	int i;
	UpdateIndex * upIndex = cc->updateIndex;
	
	int retval = -1 ;
	if(ccA > ccB){
		
		if( ccA >= (upIndex->size_of_ccUpdate) ){
			retval = reallocUpdateIndex(upIndex,ccA);
			if(retval<0)
				return retval;
		}	
	}
	else{
		
		if( ccB >= (upIndex->size_of_ccUpdate) ){
			retval = reallocUpdateIndex(upIndex,ccB);
			if(retval<0)
				return retval;
		}	
	}
	
	if( (upIndex->ccUpdate[ccA] == NULL) && (upIndex->ccUpdate[ccB] == NULL) ){
		
		upIndex->ccUpdate[ccA] = malloc( sizeof(ByteTable) );
		upIndex->ccUpdate[ccA]->ccTable = malloc( sizeof(char) * (upIndex->size_of_ccUpdate) );
		for(i=0;i<(upIndex->size_of_ccUpdate);i++)
			upIndex->ccUpdate[ccA]->ccTable[i]=0;
			
		upIndex->ccUpdate[ccB] = upIndex->ccUpdate[ccA] ;
		
		upIndex->ccUpdate[ccA]->size= upIndex->size_of_ccUpdate;
		
		upIndex->ccUpdate[ccA]->ccTable[ccB] = 1 ;
		upIndex->ccUpdate[ccB]->ccTable[ccA] = 1 ;
					
	}
	else if( (upIndex->ccUpdate[ccA] == NULL) && (upIndex->ccUpdate[ccB] != NULL) ){//if ccA=NULL and ccB not Null
		
		if(ccA >= (upIndex->ccUpdate[ccB]->size)){
			upIndex->ccUpdate[ccB]->ccTable = realloc(upIndex->ccUpdate[ccB]->ccTable,(ccA+1)*sizeof(char));
			if(upIndex->ccUpdate[ccB]->ccTable ==NULL){
				return UPDATEINDEX_REALLOC_ERROR;	
			}
			for(i=(upIndex->ccUpdate[ccB]->size);i<(ccA+1);i++)
				upIndex->ccUpdate[ccB]->ccTable[i] = 0;
				
			upIndex->ccUpdate[ccB]->size = (ccA+1) ;
		}
		upIndex->ccUpdate[ccA]=upIndex->ccUpdate[ccB]; // CcA now points to ccB's neighbor array
		
		upIndex->ccUpdate[ccA]->ccTable[ccB] = 1 ;
		upIndex->ccUpdate[ccB]->ccTable[ccA] = 1 ;
		
	}
	else if( (upIndex->ccUpdate[ccA] != NULL) && (upIndex->ccUpdate[ccB] == NULL) ){//if ccB=NULL and ccA not Null
		
		if(ccB >= (upIndex->ccUpdate[ccA]->size)){
			upIndex->ccUpdate[ccA]->ccTable = realloc(upIndex->ccUpdate[ccA]->ccTable,(ccB+1)*sizeof(char));
			if(upIndex->ccUpdate[ccA]->ccTable ==NULL){
				return UPDATEINDEX_REALLOC_ERROR;
			}
			for(i=(upIndex->ccUpdate[ccA]->size);i<(ccB+1);i++)
				upIndex->ccUpdate[ccA]->ccTable[i]= 0;
			
			upIndex->ccUpdate[ccA]->size = (ccB+1) ;	
		}
		upIndex->ccUpdate[ccB]=upIndex->ccUpdate[ccA]; // Ccb now points to cca's neighbor array
		
		upIndex->ccUpdate[ccA]->ccTable[ccB] = 1 ;
		upIndex->ccUpdate[ccB]->ccTable[ccA] = 1 ;
		
	}
	else if( (upIndex->ccUpdate[ccA] != NULL) && (upIndex->ccUpdate[ccB] != NULL) ){
		
		if( upIndex->ccUpdate[ccA] != upIndex->ccUpdate[ccB] ){

			mergeByteArrays(upIndex,ccA,ccB);   
						 
			upIndex->ccUpdate[ccA]->ccTable[ccB] = 1 ;
			upIndex->ccUpdate[ccB]->ccTable[ccA] = 1 ;
			
		}
		else{
			return ALREADY_EXIST;
		}
		
	}
	else{
		return GENERAL_ERROR;	
	}
	
	return OK_SUCCESS;

}
	
//free's smallest array
void mergeByteArrays(UpdateIndex * upIndex,uint32_t ccA,uint32_t ccB){
	

	
	if( (upIndex->ccUpdate[ccA]->size)  > (upIndex->ccUpdate[ccB]->size) ){
		
		int i;
		for(i=0; i<(upIndex->ccUpdate[ccB]->size) ; i++){
	
			upIndex->ccUpdate[ccA]->ccTable[i] = (upIndex->ccUpdate[ccA]->ccTable[i]) | (upIndex->ccUpdate[ccB]->ccTable[i]);
			if( (i!=ccB) && (upIndex->ccUpdate[ccA]->ccTable[i] == 1) )
				upIndex->ccUpdate[i]=upIndex->ccUpdate[ccA];

		}	
		free(upIndex->ccUpdate[ccB]->ccTable);
		free(upIndex->ccUpdate[ccB]);
		upIndex->ccUpdate[ccB]=upIndex->ccUpdate[ccA];
	
	}
	else{
				
		int i;
		for(i=0; i<(upIndex->ccUpdate[ccA]->size) ; i++){
	
			upIndex->ccUpdate[ccB]->ccTable[i] = (upIndex->ccUpdate[ccA]->ccTable[i]) | (upIndex->ccUpdate[ccB]->ccTable[i]);
			if( (i!=ccA) && (upIndex->ccUpdate[ccB]->ccTable[i] == 1) )
				upIndex->ccUpdate[i]=upIndex->ccUpdate[ccB];

		}	
		free(upIndex->ccUpdate[ccA]->ccTable);
		free(upIndex->ccUpdate[ccA]);
		upIndex->ccUpdate[ccA]=upIndex->ccUpdate[ccB];
	
	}

	
}	


int reallocUpdateIndex(UpdateIndex * upIndex,uint32_t ccA){
	
	int new_size_aux = (ccA+1) ;
	int old_size_aux = upIndex->size_of_ccUpdate ;
	
	void * tempUpIndex= realloc(upIndex->ccUpdate, (new_size_aux * sizeof(char*)) );
	if(upIndex->ccUpdate==NULL){
		return UPDATEINDEX_REALLOC_ERROR;
	}
	upIndex->ccUpdate = tempUpIndex;
	upIndex->size_of_ccUpdate=new_size_aux;
	
	//array initialization//
	int i;
	for(i=old_size_aux;i<new_size_aux;i++){
		upIndex->ccUpdate[i]=NULL;
	}
	
	return OK_SUCCESS;

}
	
///--------------------///

int searchForPath(CC * cc,uint32_t nodeA,uint32_t nodeB){ //this function find if a path exist, return -1 if there is not exist

	uint32_t ccA = findNodeConnectedComponentID(cc, nodeA);
	uint32_t ccB = findNodeConnectedComponentID(cc, nodeB);

	if(ccA==ccB){
		//path maybe exists
		//call bbfs with constraints for specific cc
	}
	else{ 
		//bbfs searches only to CC it needs

		int reval = findPairIntoUpdateIndex(cc,ccA,ccB);
		if(reval==EXIST){
			//path maybe exists
			//call bbfs with constraints for specific cc
		}
		else if(reval==NOT_EXIST){ //den yparxei monopati
			return NOT_EXIST;	
		}

	}
}

int metricReturn(CC * cc){
	cc->metricVal = (cc->q_that_used_ui)/(cc->all_qs);
	return (cc->metricVal);
}

int checkMetricToRebuildIndexes(CC * cc){
	if((cc->metricVal)>METRIC_VAL_LIMIT)
		return 1;
	else
		return -1;
}	

//=====================================//Estimate Connected Components//=====================================//

CC* estimateConnectedComponents(NodeIndex *ind_out, NodeIndex *ind_in, MetaDataBuffer *out, MetaDataBuffer *in) {
	uint32_t i, pos;
	uint32_t size0fBitArray = MAX(ind_out->size, ind_in->size);
	int err;
	CC *components = createConnectedComponents(size0fBitArray);
	char *dirty_byte_array = malloc(sizeof(char) * (components->size_of_ccindex));
	for(i=0; i<components->size_of_ccindex; i++)
		dirty_byte_array[i] = 0;

	for(i=0;i<components->size_of_ccindex;i++) {
		if(((ind_out->size > i) && (ind_out->index[i] != -1)) || ((ind_in->size > i)  && (ind_in->index[i] != -1))) {
			if(dirty_byte_array[i] == 0) {
				findCC(components, ind_out, ind_in, out, in, dirty_byte_array, size0fBitArray, i);
				(components->ccCounter)++;
			}
		}
	}

	free(dirty_byte_array);
	return components;

}

int findCC(CC *components, NodeIndex *ind_out, NodeIndex *ind_in, MetaDataBuffer *out, MetaDataBuffer *in, char *byte_array, uint32_t size0fBitArray, uint32_t starting_pos) {
	int retVal, i, err;
	uint32_t offset, st_size = 10000;
	uint32_t *stack = malloc(10000* sizeof(uint32_t));
	uint32_t st_pointer = 0;
	uint32_t current = starting_pos;
	
	if(byte_array[starting_pos] == 1)
		return -1;

	byte_array[current] = 1;
	err = insertIntoCCIndex( components, current, components->ccCounter);
	if( err != OK_SUCCESS ) {
		printf("ERROR:INSERT INTO CC INDEX\n");
		return -1;
	}
	while(1){
		/*========add outgoing neighbors to stack=========*/
		if( (ind_out->size > current) && (ind_out->index[current] != -1) ) {
			offset = ind_out->index[current];
			do {
				for(i=0;i<N;i++) {
					if( ((out->buffer[offset]).neighbor[i]) == -1)
						break;

					if(byte_array[(out->buffer[offset]).neighbor[i]] == 1)
						continue;

					byte_array[(out->buffer[offset]).neighbor[i]] = 1;
					err = insertIntoCCIndex( components, (out->buffer[offset]).neighbor[i], components->ccCounter);
					if( err != OK_SUCCESS ) {
						printf("ERROR:INSERT INTO CC INDEX\n");
						return -1;
					}
					stack[st_pointer] = (out->buffer[offset]).neighbor[i];	
					st_pointer++;
					if(st_pointer >= st_size){
						st_size = 2*st_size;
						stack = realloc(stack, st_size *sizeof(uint32_t));
					}
				}
				offset = (out->buffer[offset]).nextListNode;	
			}while(offset != -1);
		}
		/*========add incoming neifhbors to stack=========*/
		if( (ind_in->size > current) && (ind_in->index[current] != -1) ) {
			offset = ind_in->index[current];
			do {
				for(i=0;i<N;i++) {
					if( ((in->buffer[offset]).neighbor[i]) == -1)
						break;

					if(byte_array[(in->buffer[offset]).neighbor[i]] == 1)
						continue;
		
					byte_array[(in->buffer[offset]).neighbor[i]] = 1;
					err = insertIntoCCIndex( components, (in->buffer[offset]).neighbor[i], components->ccCounter);
					if( err != OK_SUCCESS ) {
						printf("ERROR:INSERT INTO CC INDEX\n");
						return -1;
					}				
					stack[st_pointer] = (in->buffer[offset]).neighbor[i];	
					st_pointer++;
					if(st_pointer >= st_size){
						st_size = 2*st_size;
						stack = realloc(stack, st_size *sizeof(uint32_t));
					}
					
				}
				offset = (in->buffer[offset]).nextListNode;	
			}while(offset != -1);
		}
		/*===================================*/

		st_pointer--;
		if(st_pointer == -1){
			break;
		}
		current = stack[st_pointer];
	}

	free(stack);
	return 1;

}

//===========================================================================================================//


