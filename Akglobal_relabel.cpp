#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

using namespace std;

/* graph as  a input lekar it returns index of edge connecting node 's' and  node 'e' and return the corresponding index in the edgeList and the 
edgeLengh array*/
int findIndex(int V,int E,int s,int e,int *index,int *edgeList,int *edgeLength){
    int start=index[s];
    int end=E-1;
    if(s<V) end=index[s+1]-1;
    for(int i=start;i<=end;i++){
        if(edgeList[i]==e) return i;
    }
    return -1;
}

void Akglobal_relabel(int V, int E,int source, int sink, int *cpu_height, int *cpu_excess_flow, int *Excess_total, bool *mark, bool *scanned,
    int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,
    int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,
    int *rev_cpu_index,int *rev_cpu_edgeList
)
{
    for(int u=0;u<V;u++){
        int start=cpu_index[u];
        int end=E-1;

        if(u<V)end=cpu_index[u+1]-1;

        for(int j=start;j<=end;j++){

            int v=cpu_edgeList[j];
            if(cpu_edgeLength[j]>0){
                if(cpu_height[u]>cpu_height[v]+1){
                    int index2=findIndex(V,2*E,u,v,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                    int index3=findIndex(V,2*E,v,u,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);

                    cpu_excess_flow[u] = cpu_excess_flow[u] - cpu_RedgeLength[index2];
                    cpu_excess_flow[v] = cpu_excess_flow[v] + cpu_RedgeLength[index2];
                   

                    cpu_RedgeLength[index3]+=cpu_RedgeLength[index2];
                    cpu_RedgeLength[index2]=0;
                }
            } 
        }
    }
        printf("Performing backward bfs started\n");
        // performing backwards bfs from sink and assigning height values with each vertex's BFS tree level
        
        // declaring the Queue 
        std::list<int> Que;

        // declaring variables to iterate over nodes for the backwards bfs and to store current tree level
        int x,y,current;
        
        // initialisation of the scanned array with false, before performing backwards bfs
        for(int i = 0; i < V; i++)
        {
            scanned[i] = false;
        }

        // Enqueueing the sink and set scan(sink) to true 
        Que.push_back(sink);
        scanned[sink] = true;
        cpu_height[sink] = 0;
        // bfs routine and assigning of height values with tree level values
        while(!Que.empty())
        {
            // dequeue
            x = Que.front();
            Que.pop_front();

            // capture value of current level
            current = cpu_height[x];
            
            // increment current value
            current = current + 1;

            int start=rev_cpu_index[x];
            int end=2*E-1;
            if(start<V) end=rev_cpu_index[x+1];

            for(int i=start;i<=end;i++){
                int y=rev_cpu_edgeList[i];

                int index=findIndex(V,2*E,y,x,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength);
                
                if(cpu_RedgeLength[index]>0){
                    if(scanned[y] == false){
                        //assign current as height of y node
                        cpu_height[y] = current;

                        // mark scanned(y) as true
                        scanned[y] = true;

                        // Enqueue y
                        Que.push_back(y);
                    }
                }
            }
        }
        printf("Reverse bfs ended\n");
        printf("Pre check\n");
        // declaring and initialising boolean variable for checking if all nodes are relabeled
        bool if_all_are_relabeled = true;

        for(int i = 0; i < V; i++)
        {
            if(scanned[i] == false)
            {
                if_all_are_relabeled = false;
                break;
            }
        }

        // if not all nodes are relabeled
        if(if_all_are_relabeled == false)
        {
            // for all nodes
            for(int i = 0; i < V; i++)
            {
                // if i'th node is not marked or relabeled
                if( !( (scanned[i] == true) || (mark[i] == true) ) )
                {
                    // mark i'th node
                    mark[i] = true;

                    /* decrement excess flow of i'th node from Excess_total
                     * This shows that i'th node is not scanned now and needs to be marked, thereby no more contributing to Excess_total
                     */

                    *Excess_total = *Excess_total - cpu_excess_flow[i];
                }
            }
        }

}