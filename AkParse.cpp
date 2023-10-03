#include "myheader.h"
using namespace std;


/* retuns the number of nodes and edges in a graph taking .txt file as a input*/
vector<int> whatsize(char *filepath){
    graph g(filepath);
    g.parseGraph();
    int V=g.num_nodes();
    int E=g.num_edges();
    return {V,E};
}




/* it initialises the """""CSR ARRAYS""""" of the the original graph and also the residual graph */
void readGraph(char *filepath,int *cpu_index,int *cpu_edgeList,int *cpu_edgeLength,int *cpu_Rindex,int *cpu_RedgeList,int *cpu_RedgeLength,
                int *rev_cpu_index,int* rev_cpu_edgeList)
{
    graph g(filepath),rg(filepath);
    g.parseGraph();
    rg.parseGraphResidual();
    int V=g.num_nodes();
    int E=g.num_edges();
    
    // int RV=rg.num_nodes();
    // int RE=rg.num_edges();

    // cout<<"number of nodes in the original network is "<<V<<endl;
    // cout<<"number of edges in the original network is "<<E<<endl;

    // cout<<"number of nodes in the residual network is "<<RV<<endl;
    // cout<<"number of edges in the residual network is "<<RE<<endl;


    for(int i=0;i<V;i++) cpu_index[i]=g.indexofNodes[i];
    int *p=g.getEdgeLen();
    for(int i=0;i<E;i++){
        cpu_edgeList[i]=g.edgeList[i];
        cpu_edgeLength[i]=p[i];
        
    }

    for(int i=0;i<V;i++) {
        cpu_Rindex[i]=rg.indexofNodes[i];
        rev_cpu_index[i]=rg.rev_indexofNodes[i];
    }
    int *q=rg.getEdgeLen();
    for(int i=0;i<2*E;i++){
        //cout<<"."<<p[i]<<" ";
        cpu_RedgeList[i]=rg.edgeList[i];
        cpu_RedgeLength[i]=q[i];
        //cout<<"."<<cpu_RedgeLength[i]<<"  hiiii  ";
        rev_cpu_edgeList[i]=rg.srcList[i];
        
    }
    cout<<"Parsing Completed"<<endl;
    return ;
}