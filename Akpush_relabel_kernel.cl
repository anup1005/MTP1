
#include "libOpenCL.h"
__kernel void push_relabel_kernel(int V,  int E, int source,int sink ,__global int* gpu_height, __global int* gpu_excess_flow,
      __global int* gpu_index,__global int* gpu_edgeList,__global int* gpu_edgeLength,
      __global int* gpu_Rindex,__global int* gpu_RedgeList,__global int* gpu_RedgeLength
)
 { // BEGIN KER FUN via ADDKERNEL
  unsigned int u = get_global_id(0);
  if(u<V){
    printf("kernel is called for vertex %d\n",u);
    int cycles=V;
    /* Variables declared to be used inside the kernel :
        * e_dash - initial excess flow of node u

        * h_dash - height of lowest neighbor of node u
        * h_double_dash - used to iterate among height values to find h_dash
        * * v_dash - lowest neighbor of node u 
        * v - used to iterate among nodes to find v_dash
        
        * d - flow to be pushed from node u
    */
    int e_dash,h_dash,h_double_dash,v,v_dash,d,index;
    while(cycles){
        if( (gpu_excess_flow[u] > 0) && (gpu_height[u] < V) ){
            e_dash = gpu_excess_flow[u];
            h_dash = 1e9;
            v_dash = NULL;
            //now I'll iterate through all the neighbors of v in the residual graph to find neighbor of u with lowest height
            int start=gpu_Rindex[u];
            int end=2*E-1;
            int index=-1;
            if(u<V-1) end=gpu_Rindex[u+1]-1;
            for(int i=start;i<=end;i++){
              if(gpu_RedgeLength[i]>0){
                v=gpu_RedgeList[i];
                h_double_dash=gpu_height[v];
                if(h_double_dash < h_dash)
                  {
                      v_dash = v;//my neighbow with lowest height
                      h_dash = h_double_dash; //height of the lowest neighbor
                      index=i;//index is the index from """u""" to """""v_dash""""" which will be used to change the value
                  }
              }
            }
            
            if(gpu_height[u]>h_dash){


              int start2=gpu_Rindex[v_dash];
              int end2=2*E-1;
              if(v_dash<V-1) end=gpu_Rindex[v_dash+1]-1;
              int index2=-1;
              for(int i=start2;i<=end2;i++){
                if(gpu_RedgeList[i]==u){
                  index2=i;
                  break;
                }
              }// index2 is the index of the  """""v_dash"""""  to   """u""" which will be used to change the value
              
              
              d=gpu_excess_flow[u];
              if(d>gpu_RedgeLength[index]){
                d=gpu_RedgeLength[index];
              }
              //atomic_add(&gpu_RedgeLength[4], 69);
                    // // Residual flow towards lowest neighbor from node u is increased
                    // atomicAddD(&gpu_rflowmtx[IDX(v_dash,u)],d);

                    // // Residual flow towards node u from lowest neighbor is decreased
                    // atomicSub(&gpu_rflowmtx[IDX(u,v_dash)],d);

                    // // Excess flow of lowest neighbor and node u are updated
                    // atomicAddD(&gpu_excess_flow[v_dash],d);
                    // atomicSub(&gpu_excess_flow[u],d);
              printf("pushing %d from %d to %d\n",d,u,v);
              atomic_add(&gpu_RedgeLength[index],-d);
              atomic_add(&gpu_RedgeLength[index2],d);
              atomic_add(&gpu_excess_flow[u],-d);
              atomic_add(&gpu_excess_flow[v_dash],d); 
            }
            else{
              printf("relabelling for %d\n",u);
              gpu_height[u] = h_dash + 1;
            }

        }
        cycles--;
    }

  }

 } // end KER FUNC














































 /*

  if(v >= V) return;

//ForAll started here
for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
  int u = d_data[edge];
  if (u < v){ // if filter begin 

    //ForAll started here
    for (int edge = d_meta[v]; edge < d_meta[v+1]; edge++) { // FOR NBR ITR 
      int w = d_data[edge];
      if (w > v){ // if filter begin 
        if (isNeighbour(u, w, d_meta, d_data)){ // if filter begin 
          atomic_add( triangle_count, (int)1);


        } // if filter end

      } // if filter end

    } //  end FOR NBR ITR. TMP FIX!

  } // if filter end

} //  end FOR NBR ITR. TMP FIX!
 */