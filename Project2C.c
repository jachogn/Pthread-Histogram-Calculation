/*
 ============================================================================
 Name        : Program2C.c
 Author      : Joshua Chong
 Version     : Part C
 Description : Calculate histogram program parallelized with pthreads. Each
 	 	 	   Thread has a local copy of histogram array, and passes it back
 	 	 	   to main.
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

float maxVal;
float minVal;
int resultCode;
int *histogramVals;
float binSize;		   /*bin size calculated from min and max of array values*/
float rawData[1000000];
int cnt = 1000000;

void* calcHistogram (void* arg);

struct thread{
	int id;
	int* histogram;
	float* data;
};

int main(void) {
	clock_t begin = clock();
	struct thread tid[NUM_THREADS];
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
	int points = cnt/NUM_THREADS;


	for (int index = 0; index < NUM_THREADS; index++) {
		tid[index].data = &rawData [index * points];
		tid[index].id = index;
		pthread_create(&thread[index], NULL, calcHistogram, &tid[index]);
	}
	for (int index = 0; index < NUM_THREADS; index++) {
		// block until thread 'index' completes
		pthread_join(thread[index], NULL);
	}
	for (int cnt1 = 0; cnt1 < NUMBINS; cnt1++){
		//printf("bin: %d  binStart: %f   binEnd:  %f  Histogram Value: %d\n", cnt1, binSize*(float)cnt1, binSize*(float)(cnt1+1), histogramVals[cnt1]);
	}

	/* Calculating time it take to run the program */
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Time in seconds: %f", time_spent);
	return EXIT_SUCCESS;
}




/*-------------------------------------------------------------------
 * Function:    calcHistogram
 * Purpose:     Calculate Histogram result is in a global variable histogramVals.
 * In args:     data:  values to be histogrammed.
 *              numDataPoints:  Number of data points to be histogrammed.
 *              numBins:  Number of bins in output histogram
 * Outputs:     Histogram of values stored in global histogramVals
				binSize in global variable.
 *
 */

void* calcHistogram (void* arg) {
	struct thread* thd = (struct thread*) arg;
	float* data = thd->data;

	for(int i = 0; i < cnt/NUM_THREADS; i++){
		int binNUM = (int) ((data[i] - minVal)/binSize);
		thd->histogram[binNUM]++;
	}
}


