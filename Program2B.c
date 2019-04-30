/*
 ============================================================================
 Name        : Program2B.c
 Author      : Joshua Chong
 Version     : Part B
 Description : Calculate histogram program parallelized with pthreads.
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <time.h>
#include <pthread.h>

#define NUMBINS 1000
#define NUM_THREADS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int *histogramVals;    /*global histogram array for all threads*/
float binSize;		   /*bin size calculated from min and max of array values*/
int numDataPoints;
float rawData[1000000];
int cnt = 1000000;
int resultCode;
float maxVal;
float minVal;

void* calcHistogram (void* arg);

int main(void) {
	clock_t begin = clock();
	histogramVals =  malloc(NUMBINS * sizeof(int));

	FILE *fileIn = fopen("numbers.bin", "rb");
	resultCode = fread(rawData, sizeof(rawData[0]), sizeof(rawData)/sizeof(rawData[0]), fileIn);
	maxVal = FLT_MIN;
	minVal = FLT_MAX;
	for(int cnt1 = 0; cnt1 < cnt; cnt1++ ) {
		if ( rawData[cnt1] > maxVal )
			maxVal = rawData[cnt1];
		if ( rawData[cnt1] < minVal )
			minVal = rawData[cnt1];
		}
	binSize = (maxVal-minVal)/(float)NUMBINS;

	if (fileIn == NULL){
		printf("File not opened\n");
		return 0;
	}
	printf("minVal: %f  maxVal: %f  binSize: %f\n", minVal, maxVal, binSize);
	fclose(fileIn);

	pthread_t thread[NUM_THREADS];
	int index;
	for (index = 0; index < NUM_THREADS; index++) {
		//creating threads
		pthread_create(&thread[index], NULL, calcHistogram, &rawData[index * (cnt/NUM_THREADS)]);
	}
	for (index = 0; index < NUM_THREADS; index++) {
		// block until thread 'index' completes
		pthread_join(thread[index], NULL);
	}
	for (int cnt1 = 0; cnt1 < NUMBINS; cnt1++){
		 //printf("bin: %d  binStart: %f   binEnd:  %f  Histogram Value: %d\n", cnt1, binSize*(float)cnt1, binSize*(float)(cnt1+1), histogramVals[cnt1]);
	}

	/* Calculating the time the program takes to run */
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time in seconds: %f", time_spent);
	return EXIT_SUCCESS;
}



/*-------------------------------------------------------------------
 * Function:    calcHistogram
 * Purpose:     Calculate Histogram result is in a global variable histogramVals.
 * In args:     arg: changed to one parameter in order to fit pthread_create
 * 				arg is the starting points of each thread. Example: 0, 250000, 500000,
 * 				1000000.
 * Outputs:     Histogram of values stored in global histogramVals
				binSize in global variable.
 *
 */

void* calcHistogram (void* arg) {
	float* data = (float*)arg;
	for (int i = 0; i < cnt/NUM_THREADS ; i++){
		int binNum = (int)((data[i] - minVal) / binSize);
		pthread_mutex_lock(&mutex);
		histogramVals[binNum]++;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}


