/**
 *  @file test_compress_ts.c
 *  @author Sheng Di
 *  @date May, 2018
 *  @brief This is an example of using compression interface
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */


#include <stdio.h>
#include <stdlib.h>
#include "sz.h"
#include "rw.h"

struct timeval startTime;
struct timeval endTime;  /* Start and end times */
struct timeval costStart; /*only used for recording the cost*/
double totalCost = 0;

#define NB_variable 6


void cost_start()
{
	totalCost = 0;
        gettimeofday(&costStart, NULL);
}

void cost_end()
{
        double elapsed;
        struct timeval costEnd;
        gettimeofday(&costEnd, NULL);
        elapsed = ((costEnd.tv_sec*1000000+costEnd.tv_usec)-(costStart.tv_sec*1000000+costStart.tv_usec))/1000000.0;
        totalCost += elapsed;
}


int main(int argc, char * argv[])
{
    int i = 0;
    size_t r5=0,r4=0,r3=0,r2=0,r1=0;
    char oriDir[640], outputDir[640], outputFilePath[600];
    char *cfgFile;
    float eb = 0.001;
    float eb2 = 0.01;
    
    if(argc < 3)
    {
		printf("Test case: testfloat_compress_ts [config_file] [srcDir] [dimension sizes...]\n");
		printf("Example: testfloat_compress_ts sz.config /home/sdi/Data/Hurricane-ISA/consecutive-steps 500 500 100\n");
		exit(0);
    }
   
    cfgFile=argv[1];
    sprintf(oriDir, "%s", argv[2]);//add a comment to test github
    if(argc>=4)
		r1 = atoi(argv[3]); //8
    if(argc>=5)
		//r2 = atoi(argv[4]); //8
        eb = atof(argv[4]);
    if(argc>=6)
		//r3 = atoi(argv[5]); //128
        eb2 = atof(argv[5]);
    if(argc>=7)
        r4 = atoi(argv[6]);
    if(argc>=8)
        r5 = atoi(argv[7]);
   
    printf("cfgFile=%s\n", cfgFile); 
    int status = SZ_Init(cfgFile);
    if(status == SZ_NSCS)
		exit(0);
    sprintf(outputDir, "%s", oriDir);
   
    char oriFilePath[600];
    size_t nbEle;
    size_t dataLength = computeDataLength(r5,r4,r3,r2,r1);
    //float *data = (float*)malloc(sizeof(float)*dataLength);
    float **data = (float**) malloc(NB_variable * sizeof(float*));
    for (i = 0; i < NB_variable; i++){
        data[i] = (float*) malloc(dataLength * sizeof(float));
    }
    int64_t* index = (int64_t*) malloc(dataLength * sizeof(int64_t));
    //SZ_registerVar("CLOUDf", SZ_FLOAT, data, REL, 0, 0.001, 0, r5, r4, r3, r2, r1);

    SZ_registerVar("x", SZ_FLOAT, data[0], REL, 0, eb, 0, r5, r4, r3, r2, r1);
    SZ_registerVar("y", SZ_FLOAT, data[1], REL, 0, eb, 0, r5, r4, r3, r2, r1);
    SZ_registerVar("z", SZ_FLOAT, data[2], REL, 0, eb, 0, r5, r4, r3, r2, r1);
    SZ_registerVar("vx", SZ_FLOAT, data[3], PW_REL, 0, 0, eb2, r5, r4, r3, r2, r1);
    SZ_registerVar("vy", SZ_FLOAT, data[4], PW_REL, 0, 0, eb2, r5, r4, r3, r2, r1);
    SZ_registerVar("vz", SZ_FLOAT, data[5], PW_REL, 0, 0, eb2, r5, r4, r3, r2, r1);
    //SZ_registerVar("vx", SZ_FLOAT, data[3], REL, 0, eb2, 0, r5, r4, r3, r2, r1);
    //SZ_registerVar("vy", SZ_FLOAT, data[4], REL, 0, eb2, 0, r5, r4, r3, r2, r1);
    //SZ_registerVar("vz", SZ_FLOAT, data[5], REL, 0, eb2, 0, r5, r4, r3, r2, r1);
    SZ_registerVar("index", SZ_INT64, index, REL, 0, eb, 0, r5, r4, r3, r2, r1);

    if(status != SZ_SCES)
    {
		printf("Error: data file %s cannot be read!\n", oriFilePath);
		exit(0);
    }

    int file_num[6] = {100, 102, 105, 107, 110, 113};
    //int file_num[101] = {42, 43, 44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 59, 60, 62, 63, 65, 67, 68, 70, 72, 74, 76, 77, 79, 81, 84, 86, 88, 90, 92, 95, 97, 100, 102, 105, 107, 110, 113, 116, 119, 121, 124, 127, 131, 134, 137, 141, 144, 148, 151, 155, 159, 163, 167, 171, 176, 180, 184, 189, 194, 198, 203, 208, 213, 219, 224, 230, 235, 241, 247, 253, 259, 266, 272, 279, 286, 293, 300, 307, 315, 323, 331, 338, 347, 355, 365, 373, 382, 392, 401, 411, 421, 432, 442, 453, 464, 475, 487, 499};
   
    size_t outSize; 
    unsigned char *bytes = NULL;
    for(i=0;i<6;i++)
	{
		printf("simulation time step %d\n", i);
        int m = 0;
        for (m = 0; m < NB_variable; m++){
            sprintf(oriFilePath, "%s/m000.full.mpicosmo.%d#21-%d.dat", oriDir, file_num[i], m);
            float* data_ = readFloatData(oriFilePath, &nbEle, &status);
            memcpy(data[m], data_, nbEle*sizeof(float));
            free(data_);
        }
        sprintf(oriFilePath, "%s/m000.full.mpicosmo.%d#21-i.dat", oriDir, file_num[i]);
        int64_t* index_ = readInt64Data(oriFilePath, &nbEle, &status);
        memcpy(index, index_, nbEle*sizeof(int64_t));


        //sihuan updated the variable dimensions
        SZ_Variable* v = NULL;
        v = sz_varset->header->next;
        int loop;
        for (loop = 0; loop < 6; loop++){
            v->r1 = nbEle;
            v=v->next;
        }
        printf("the updated r1 is: %zu\n", sz_varset->header->next->r1);




        //printf("--------------- %lld \n", index_[0]);
		//float *data_ = readFloatData(oriFilePath, &nbEle, &status);
		//memcpy(data, data_, nbEle*sizeof(float));
		cost_start();
		SZ_compress_ts_vlct(&bytes, &outSize);
		cost_end();
		printf("timecost=%f\n",totalCost); 
		sprintf(outputFilePath, "%s/QCLOUDf%02d.bin.dat.sz2", outputDir, i);
		printf("writing compressed data to %s\n", outputFilePath);
		writeByteData(bytes, outSize, outputFilePath, &status); 
		free(bytes);
		//free(data_);
	}
    sprintf(outputFilePath, "%s/delta_t_opt_output.txt", outputDir);
    writeFloatData(delta_t_opt, 5, outputFilePath, &status);

    float overall_cmp_ratio = 0.0;
    for (i = 0; i < 6; i++){
        overall_cmp_ratio+=1.0/cmp_ratio[i];
    }
    printf("The overall compration for 6 steps is: %.5f\n", 6.0/overall_cmp_ratio);
    
    printf("done\n");
    free(data);
    SZ_Finalize();
    
    return 0;
}
