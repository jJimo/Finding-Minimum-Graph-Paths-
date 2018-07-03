#ifndef ccbfs
#define ccbfs

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include "connectedComponents.h"
#include "bfs.h"

#define ETERNAL_ERROR 1

int  examine_reachable_nodes_DYNAMIC(char * dirtybit_array_a,char * dirtybit_array_b,CC *cc, neighbors* nb);
int bfs_CC(CC *cc, uint32_t nodeA,uint32_t nodeB,char * dirty_incoming,char* dirty_outcoming,neighbors* forward_neighbors,neighbors* backward_neighbors);

#endif
