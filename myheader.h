#ifndef myheader_h
#define myheader_h

#include <CL/cl.h>
#include "graph.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Akglobal_relabel.cpp"
#include "AkParse.cpp"
#include "Akpreflow.cpp"
// #include "Akpush_relabel.cpp"
//#include "Akpush_relabel_kernel.cl"






#define number_of_nodes 6
#define threads_per_block 256
#define number_of_blocks_nodes ((number_of_nodes/threads_per_block) + 1)
#define number_of_blocks_edges ((number_of_edges/threads_per_block) + 1)
#define INF 1000000000

#endif