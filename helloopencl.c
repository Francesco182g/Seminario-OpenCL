#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
 
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)
 
int main()
{
/* Inizializzazione Elementi */
cl_platform_id platform_id = NULL; 
cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue command_queue = NULL;
cl_mem memobj = NULL;
cl_program program = NULL;
cl_kernel kernel = NULL; 
cl_uint ret_num_platforms; //Get delle piattaforme disponibili
cl_uint ret_num_devices; //Get dei device disponibili

cl_int ret;
 
char string[MEM_SIZE];
 
FILE *fp;
char fileName[] = "./hello.cl";
char *source_str;
size_t source_size;
 
/* Caricare il file contenente il kernel.cl*/
fp = fopen(fileName, "r");
if (!fp) {
fprintf(stderr, "Impossibile caricare il KERNEL! File non trovato o non compatibile! \n");
exit(1);
}
source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
fclose(fp);
 
/* Get Info Platform and Device */
ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms); //L'host seleziona la piattaforma da utilizzare;
ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices); //L'host selezione il dveice da utilizzare, CL__..._DEFAULT;
 
/* Crea il contex OpenCL */
context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
 
/* Crea la Coda dei Comandi */
command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
 
/* Crea il buffer di Memoria */
memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);
 
/* Carico il kernel dalla sorgente .cl */
program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
(const size_t *)&source_size, &ret);
 
/* Effettuo il build del kernel  */
ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
 
/* Creo il kernel OpenCL*/
kernel = clCreateKernel(program, "hello", &ret);
 
/* Setto i paramentri del Kernel */
ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
 
/* Eseguo il Kernel */
ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
 
/* Copio i risultati dal buffer alla memoria host */
ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
MEM_SIZE * sizeof(char),string, 0, NULL, NULL);

/* Stampa i risultati */
printf("Hello World!\n");
printf(string);

/* Libero la Memoria  */
ret = clFlush(command_queue);
ret = clFinish(command_queue);
ret = clReleaseKernel(kernel);
ret = clReleaseProgram(program);
ret = clReleaseMemObject(memobj);
ret = clReleaseCommandQueue(command_queue);
ret = clReleaseContext(context);

free(source_str);
 
return 0;
}
