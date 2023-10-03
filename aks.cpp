#include "myheader.h"
using namespace std;



int main (int argc, char **argv) {
    vector<int> mysize=whatsize(argv[1]);
    int V=mysize[0];
    int E=mysize[1];
    int source=0;
    int sink=2;

    int *cpu_index,*cpu_edgeList,*cpu_edgeLength,*cpu_Rindex,*cpu_RedgeList,*cpu_RedgeLength;// cpu network and cpu residual graph
    //int *gpu_index,*gpu_edgeList,*gpu_edgeLength,*gpu_Rindex,*gpu_RedgeList,*gpu_RedgeLength;// gpu network and gpu residual graph



    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$Inneighbors CSR$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
    int *rev_cpu_index,*rev_cpu_edgeList;
    rev_cpu_index=(int*)malloc((V) * sizeof(int));
    rev_cpu_edgeList=(int*)malloc(2*(E) *sizeof(int));


    
    cout<<"number of provinces in the  country are"<<V<<endl;
    cout<<"number of pipes in the country are"<<E<<endl;
    
    cpu_index=(int*)malloc((V+1) * sizeof(int));
    cpu_edgeList=(int*)malloc((E) * sizeof(int));
    cpu_edgeLength=(int*)malloc((E) *sizeof(int));

    cpu_Rindex=(int*)malloc((V+1) * sizeof(int));
    cpu_RedgeList=(int*)malloc(2*(E) *sizeof(int));
    cpu_RedgeLength=(int*)malloc(2*(E) *sizeof(int));
    
    
    // int source=atoi(argv[2]);
    // int sink=atoi(argv[3]);

    // declaring variables to store graph data on host as well as on CUDA device global memory 
    int *cpu_height;
    //int *gpu_height;
    int *cpu_excess_flow;
    //int *gpu_excess_flow;
    int *Excess_total;

    cpu_height = (int*)malloc(V*sizeof(int));
    cpu_excess_flow = (int*)malloc(V*sizeof(int));
    Excess_total = (int*)malloc(sizeof(int));


    // cout<<"hello before  printing the residual weights"<<endl;
    // for(int i=0;i<(*E)*2;i++) cout<<cpu_RedgeLength[i]<<" ";
    //cout<<endl;


    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ GET THE CSR  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
    readGraph(argv[1],  cpu_index,cpu_edgeList,cpu_edgeLength, 
            cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
            rev_cpu_index,rev_cpu_edgeList);


    //^^^^^^^^^^^^^^this code is to check inneighbors CSR arrays^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // cout<<"checking for inneighbors startd"<<endl;
    // for(int i=0;i<V;i++) cout<<rev_cpu_index[i]<<"  ";
    // cout<<endl;
    // for(int i=0;i<2*E;i++) cout<<rev_cpu_edgeList[i]<<"  ";
    // cout<<endl;
    // cout<<"checking for inneighbors endd"<<endl;




    cout<<"Calling preflow"<<endl;
    /*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  SET THE PREFLOW    $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
    preflow(V,E ,source, sink,cpu_index,cpu_edgeList,cpu_edgeLength,
            cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,cpu_height,cpu_excess_flow,Excess_total );

    cout<<"Excess Total after preflow is "<<*Excess_total<<endl;

    //cout<<" is starting open CL"<<endl;

    /************************ This code is to print CSR of residual Network********************************/
        // cout<<V<<"number of nodes"<<endl;
        // cout<<E<<"number of edges"<<endl;
        // cout<<"BUILDING RESIDUAL NETWORK"<<endl;
        // cout<<"printing th indexof nodes"<<endl;
        // for(int i=0;i<V;i++) cout<<cpu_Rindex[i]<<"  ";
        // cout<<endl<<"printing edgelist"<<endl;
        // for(int i=0;i<(E)*2;i++) cout<<cpu_RedgeList[i]<<" ";
        // cout<<endl;
        // cout<<"Now printing the residual weights"<<endl;
        // for(int i=0;i<(E)*2;i++) cout<<cpu_RedgeLength[i]<<" ";
        // cout<<endl;
    
    /*******************************************Ending***************************************************************/


    /*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^OPENCL STARTA^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

        //Getting platforms
        cl_int status;
        cl_platform_id *platforms = NULL;
        cl_uint number_of_platforms;
        status = clGetPlatformIDs(0, NULL, &number_of_platforms);
        platforms = (cl_platform_id *)malloc(number_of_platforms*sizeof(cl_platform_id));
        status = clGetPlatformIDs(number_of_platforms, platforms, NULL);
        printf("Got Platform, status = %d \n", status);

        //Getting Devices present on platform
        cl_device_id *devices= NULL;
        cl_uint number_of_devices;
        status = clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_GPU, 0, NULL, &number_of_devices);
        devices = (cl_device_id *)malloc(number_of_devices*sizeof(cl_device_id));
        status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, number_of_devices, devices, NULL);
        printf("Got Devices, status = %d \n", status);

        //Creating context
        cl_context context;
        context = clCreateContext(NULL, number_of_devices, devices, NULL, NULL, &status);

        //Creating command queue
        cl_command_queue command_queue ;
        command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE , &status);
        printf("command queue created, status = %d\n", status);

        cout<<" atleast opencl startted"<<endl<<endl;






    //  cl_mem d_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (1+V)*sizeof(int), NULL, &status);
    //cl_mem d_rev_meta = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    cl_mem gpu_index = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    //printf("ccreating success, status = %d\n", status);
    cl_mem gpu_edgeList = clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_edgeLength= clCreateBuffer(context, CL_MEM_READ_WRITE, (E)*sizeof(int), NULL, &status);
     //printf("ccreating success, status = %d\n", status);

    cl_mem gpu_Rindex = clCreateBuffer(context, CL_MEM_READ_WRITE, (V+1)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_RedgeList = clCreateBuffer(context, CL_MEM_READ_WRITE, (2*E)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_RedgeLength= clCreateBuffer(context, CL_MEM_READ_WRITE, (2*E)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);

    cl_mem gpu_height= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
    // printf("ccreating success, status = %d\n", status);
    cl_mem gpu_excess_flow= clCreateBuffer(context, CL_MEM_READ_WRITE, (V)*sizeof(int), NULL, &status);
   //  printf("ccreating success, status = %d\n", status);



    //status = clEnqueueWriteBuffer(command_queue,   d_meta , CL_TRUE, 0, sizeof(int)*(V+1),   h_meta, 0, NULL, NULL );
    status = clEnqueueWriteBuffer(command_queue,   gpu_index , CL_TRUE, 0, sizeof(int)*(V+1),   cpu_index, 0, NULL, NULL );
    // printf("copying success, status = %d\n", status);
     status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
    printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}

    status = clEnqueueWriteBuffer(command_queue,   gpu_edgeList , CL_TRUE, 0, sizeof(int)*E,   cpu_edgeList, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
    //printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}

    status = clEnqueueWriteBuffer(command_queue,    gpu_edgeLength , CL_TRUE, 0, sizeof(int)*E,    cpu_edgeLength, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
   // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}


    status = clEnqueueWriteBuffer(command_queue, gpu_Rindex , CL_TRUE, 0, sizeof(int)*(V+1), cpu_Rindex, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
   // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}

    status = clEnqueueWriteBuffer(command_queue, gpu_RedgeList , CL_TRUE, 0, sizeof(int)*(2*E), cpu_RedgeList, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
    printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}

    status = clEnqueueWriteBuffer(command_queue, gpu_RedgeLength , CL_TRUE, 0, sizeof(int)*(2*E), cpu_RedgeLength, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
   // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}



    status = clEnqueueWriteBuffer(command_queue,   gpu_height , CL_TRUE, 0, sizeof(int)*(V),   cpu_height, 0, NULL, NULL );
   // printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
   // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}

    status = clEnqueueWriteBuffer(command_queue,   gpu_excess_flow , CL_TRUE, 0, sizeof(int)*(V),   cpu_excess_flow, 0, NULL, NULL );
    //printf("copying success, status = %d\n", status);
    status = clEnqueueWriteBuffer(command_queue, gpu_index, CL_TRUE, 0, sizeof(int)*(V+1), cpu_index, 0, NULL, NULL);
if (status != CL_SUCCESS) {
   // printf("Error: Failed to write buffer to device! Status Code: %d\n", status);
    // Handle the error, possibly return or exit the program
}


    cout<<"CSR transfer to OpenCl memory completed\n"<<endl;
    // /**********************************************CSR TRANSFER END**********************************************************************/

    // //->->->->->->->->->->->->->->->->->->>->->->->->->->->->->-> push_relabel()<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-
    // push_relabel(V,E ,source,sink, cpu_height, cpu_excess_flow,gpu_height, gpu_excess_flow, Excess_total,
    //         cpu_index,cpu_edgeList,cpu_edgeLength,cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
    //         gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength,
    //         rev_cpu_index,rev_cpu_edgeList);


    // /* now I'll be having the maximum flow of the of the network as """""""""""""""""cpu_excess_flow""""""""""""""""""*/
    //     cout<<"this is my final answer    ";
    //     cout<<cpu_excess_flow[sink]<<endl;



    /********************************Testing Purpose******************************************************************************/

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

    int sss=1;
    
    while(   (  (cpu_excess_flow[source] + cpu_excess_flow[sink]) < *Excess_total) && sss++ )
    {   
        if(sss>100) break;
        cout<<"************again inside the main loop for  "<< sss<<"    time  ********\n ";
        cout<<"cpu_excess_flow[source]   "<<cpu_excess_flow[source]<<endl;
        cout<<"cpu_excess_flow[sink]    "<<cpu_excess_flow[sink]<<endl;
        cout<<"*Excess_total   "<<*Excess_total<<endl;
        printf("Invoking kernel\n");
        // copying height values to OPENCl device global memory
        //cudaMemcpy(gpu_height,cpu_height,V*sizeof(int),cudaMemcpyHostToDevice);
        status = clEnqueueWriteBuffer(command_queue,   gpu_height , CL_TRUE, 0, sizeof(int)*(V),   cpu_height, 0, NULL, NULL );

        
        
//####################################################launching the kernel#####################################################################

        // push_relabel_kernel<<<number_of_blocks_nodes,threads_per_block>>>
        // (V,E,source,sink ,gpu_height,gpu_excess_flow,gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength);
        // clFinish(command_queue);
          //Reading kernel file

        cl_event event;



        FILE* kernelfp = fopen("Akpush_relabel_kernel.cl", "rb"); 
        size_t program_size;
        fseek(kernelfp, 0, SEEK_END);
        program_size = ftell(kernelfp);
        rewind(kernelfp);
        char* kernelSource;
        kernelSource = (char *)malloc((program_size+1)* sizeof(char));
        fread(kernelSource, sizeof(char), program_size, kernelfp);
        kernelSource[program_size] = (char)NULL ;
        fclose(kernelfp);

          //Creating program from source(Create and build Program)
        cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelSource, NULL, &status);
        printf("Progran created from source, status = %d \n", status);
        status = clBuildProgram(program, number_of_devices, devices, " -I ./", NULL, NULL);
        printf(" Program building completed, status = %d \n ",status);

          //Variable for launch configuration
        size_t global_size;
        size_t local_size;
        cl_kernel pr_kernel = clCreateKernel(program, "push_relabel_kernel", &status);
        if(status != CL_SUCCESS){
                char build_log[4096];
                clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
                printf("Build log:\n%s\n", build_log);
                printf("Program building failed, status = %d \n", status);
                return 0 ;
        }
        else{
            printf("AKS     successful to create push_relabel_kernel.\n");
        }

//V,E,source,sink ,gpu_height,gpu_excess_flow,gpu_index,gpu_edgeList,gpu_edgeLength,gpu_Rindex,gpu_RedgeList,gpu_RedgeLength
        status = clSetKernelArg(pr_kernel,0, sizeof(int),(void *)&V);
        status = clSetKernelArg(pr_kernel,1, sizeof(int),(void *)&E);
        status = clSetKernelArg(pr_kernel,2, sizeof(int),(void *)&source);
        status = clSetKernelArg(pr_kernel,3, sizeof(int),(void *)&sink);
        status = clSetKernelArg(pr_kernel,4, sizeof(cl_mem),(void *)&gpu_height);
        status = clSetKernelArg(pr_kernel,5, sizeof(cl_mem),(void *)&gpu_excess_flow);

        status = clSetKernelArg(pr_kernel,6, sizeof(cl_mem),(void *)&gpu_index);
        status = clSetKernelArg(pr_kernel,7, sizeof(cl_mem),(void *)&gpu_edgeList);
        status = clSetKernelArg(pr_kernel,8, sizeof(cl_mem),(void *)&gpu_edgeLength);

        status = clSetKernelArg(pr_kernel,9, sizeof(cl_mem),(void *)&gpu_Rindex);
        status = clSetKernelArg(pr_kernel,10, sizeof(cl_mem),(void *)&gpu_RedgeList);
        status = clSetKernelArg(pr_kernel,11, sizeof(cl_mem),(void *)&gpu_RedgeLength);

        local_size = 128;
        global_size = (V + local_size -1)/ local_size * local_size;

        status = clEnqueueNDRangeKernel(command_queue,pr_kernel, 1,NULL, &global_size, &local_size , 0,NULL,&event);
        clWaitForEvents(1,&event);


////###########################################################################################################################################

        // copying height, excess flow and residual flow values from device to host memory
        // cudaMemcpy(cpu_height,gpu_height,V*sizeof(int),cudaMemcpyDeviceToHost);
        // cudaMemcpy(cpu_excess_flow,gpu_excess_flow,V*sizeof(int),cudaMemcpyDeviceToHost);


        // Assuming you have created OpenCL buffers gpu_height, gpu_excess_flow, gpu_Rindex, gpu_RedgeList, gpu_RedgeLength
        // and corresponding host arrays cpu_height, cpu_excess_flow, cpu_Rindex, cpu_RedgeList, cpu_RedgeLength


       // ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
        status = clEnqueueReadBuffer(command_queue, gpu_height, CL_TRUE, 0, sizeof(int) * V, cpu_height, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_excess_flow, CL_TRUE, 0, sizeof(int) * V, cpu_excess_flow, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_Rindex, CL_TRUE, 0, sizeof(int) * (V+1), cpu_Rindex, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_RedgeList, CL_TRUE, 0, sizeof(int) * 2 * E, cpu_RedgeList, 0, NULL, NULL);
       // printf("Reading success, status = %d\n", status);
        status = clEnqueueReadBuffer(command_queue, gpu_RedgeLength, CL_TRUE, 0, sizeof(int) * 2 * E, cpu_RedgeLength, 0, NULL, NULL);
        //printf("Reading success, status = %d\n", status);


        //printf("Before Global relabel\n");
        //print(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        printf("Before Global relabel Excess total : %d\n",*Excess_total);



        // perform the global_relabel routine on host
        Akglobal_relabel(V,E,source,sink,cpu_height,cpu_excess_flow,Excess_total,mark,scanned,
                        cpu_index,cpu_edgeList,cpu_edgeLength,
                        cpu_Rindex,cpu_RedgeList,cpu_RedgeLength,
                        rev_cpu_index,rev_cpu_edgeList);

        //printf("\nAfter global relabel\n");
        //print(V,cpu_height,cpu_excess_flow,cpu_rflowmtx,cpu_adjmtx);
        printf("After global relabel Excess total : %d\n",*Excess_total);
    }
    cout<<endl<<endl;
    cout<<"Anup Kumar Singh your answer is  "<<cpu_excess_flow[sink]<<endl;

//closing for main
}
