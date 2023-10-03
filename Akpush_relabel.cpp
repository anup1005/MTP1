#include "myheader.h"

using namespace std;
void push_relabel(int V,int E ,int source, int sink, int *cpu_height, int *cpu_excess_flow,int *gpu_height, int *gpu_excess_flow,  int *Excess_total,
int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,
int *gpu_index,int *gpu_edgeList,int *gpu_edgeLength,int *gpu_Rindex,int *gpu_RedgeList,int *gpu_RedgeLength,
int *rev_cpu_index,int *rev_cpu_edgeList
)


{
    /* Instead of checking for overflowing vertices(as in the sequential push relabel),
     * sum of excess flow values of sink and source are compared against Excess_total 
     * If the sum is lesser than Excess_total, 
     * it means that there is atleast one more vertex with excess flow > 0, apart from source and sink
     */

    /* declaring the mark and scan boolean arrays used in the global_relabel routine outside the while loop 
     * This is not to lose the mark values if it goes out of scope and gets redeclared in the next iteration 
     */

    bool *mark,*scanned;
    mark = (bool*)malloc(V*sizeof(bool));
    scanned = (bool*)malloc(V*sizeof(bool));

    // initialising mark values to false for all nodes
    for(int i = 0; i < V; i++)
    {
        mark[i] = false;
    }

    while((cpu_excess_flow[source] + cpu_excess_flow[sink]) < *Excess_total)
    {
        // copying height values to OPENCl device global memory
        //cudaMemcpy(gpu_height,cpu_height,V*sizeof(int),cudaMemcpyHostToDevice);
        status = clEnqueueWriteBuffer(command_queue,   gpu_height , CL_TRUE, 0, sizeof(int)*(V),   cpu_height, 0, NULL, NULL );

        printf("Invoking kernel\n");

        // invoking the push_relabel_kernel
        //push_relabel_kernel<<<number_of_blocks_nodes,threads_per_block>>>(V,source,sink,gpu_height,gpu_excess_flow,gpu_adjmtx,gpu_rflowmtx);
        push_relabel_kernel<<<number_of_blocks_nodes,threads_per_block>>>
        (V,E,source,sink ,gpu_height,gpu_excess_flow,gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength);

        //cudaDeviceSynchronize();
        clFinish(command_queue);


        // copying height, excess flow and residual flow values from device to host memory
        // cudaMemcpy(cpu_height,gpu_height,V*sizeof(int),cudaMemcpyDeviceToHost);
        // cudaMemcpy(cpu_excess_flow,gpu_excess_flow,V*sizeof(int),cudaMemcpyDeviceToHost);
        // cudaMemcpy(cpu_rflowmtx,gpu_rflowmtx,V*V*sizeof(int),cudaMemcpyDeviceToHost);
        status = clEnqueueWriteBuffer(command_queue,   cpu_height , CL_TRUE, 0, sizeof(int)*(V),   gpu_height, 0, NULL, NULL );
        status = clEnqueueWriteBuffer(command_queue,   cpu_excess_flow , CL_TRUE, 0, sizeof(int)*(V),   gpu_excess_flow, 0, NULL, NULL );

        status = clEnqueueWriteBuffer(command_queue,   cpu_Rindex , CL_TRUE, 0, sizeof(int)*(V),   gpu_Rindex, 0, NULL, NULL );
        status = clEnqueueWriteBuffer(command_queue,   cpu_RedgeList , CL_TRUE, 0, sizeof(int)*(2*E),   gpu_RedgeList, 0, NULL, NULL );
        status = clEnqueueWriteBuffer(command_queue,   cpu_RedgeLength , CL_TRUE, 0, sizeof(int)*(2*E),   gpu_RedgeLength, 0, NULL, NULL);

        printf("After invoking\n");
        //print(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        printf("Excess total : %d\n",*Excess_total);
        // perform the global_relabel routine on host
        Akglobal_relabel(V,E,source,sink,cpu_height,cpu_excess_flow,Excess_total,mark,scanned
                        cpu_index,cpu_edgeList,cpu_edgeLength,
                        cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
                        rev_cpu_index,rev_cpu_edgeList);

        printf("\nAfter global relabel\n");
        //print(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        printf("Excess total : %d\n",*Excess_total);
    }

}