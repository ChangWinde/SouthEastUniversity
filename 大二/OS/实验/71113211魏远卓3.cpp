#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#define PTHNUM 5
#define RUSUME 3
#define NOTFINISHED 0
#define FINISHED 1
#define UNSAFE 0
#define SAFE 1
int available[RUSUME]={3,3,2};
int max[PTHNUM][RUSUME]={7,5,3,3,2,2,9,0,2,2,2,2,4,3,3};
int allocation[PTHNUM][RUSUME]={0,1,0,2,0,0,3,0,2,2,1,1,0,0,2};
int need[PTHNUM][RUSUME];
int finish[PTHNUM]={NOTFINISHED};
