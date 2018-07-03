#ifndef bfsD
#define bfsD

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include "index.h"
#include "byte_array_handler.h"
#define MOVE_ON 1
#define FOUND_PATH 2
#define NO_PATH -1



int  examine_reachable_nodes(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb);
int  examine_reachable_nodes_version(char * dirtybit_array_a,char * dirtybit_array_b,NodeIndex * indexMeta,MetaDataBuffer * mBuffer,neighbors* nb,uint32_t query_version);

int bfs(NodeIndex * incomingIndexMeta,MetaDataBuffer *incomingBuffer,NodeIndex * outcomingIndexMeta,MetaDataBuffer * outcomingBuffer, uint32_t nodeA,uint32_t nodeB,uint32_t query_version,DirtyByteArray * dirty_incoming,DirtyByteArray* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors);

#endif
