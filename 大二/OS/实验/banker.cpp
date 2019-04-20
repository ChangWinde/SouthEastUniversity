#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
using namespace std;
#define pthnum 5
#define resource 3
#define SAFE true
#define UNSAFE false


int Max[pthnum][resource]={
	{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}
};
int allocation[pthnum][resource]={
	{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}
};
int available[resource]={3,3,2};
int finished[pthnum]={0};
int need[pthnum][resource]={0};
sem_t mutex;

bool compare_work(int *b,int i){
	for(int j=0;j<resource;j++){
		if(need[i][j]<=b[j])
			continue;
		else
			return false;
	}
	return true;
}
bool compare_need(int *request,int pthnumber){
	for(int i=0;i<resource;i++)
	{
		if(request[i]<=need[pthnumber][i])
			continue;
		else
			return false;
	}
	return true;
}
bool compare_available(int *request){
	for(int i=0;i<resource;i++)
	{
		if(request[i]<=available[i])
			continue;
		else
			return false;
	}
}
bool safety(){
	int work[resource]={0};
	for(int j=0;j<resource;j++)
	{
		work[j]=available[j];
	}
	for(int k=0;k<pthnum;k++){
		for(int i=0;i<pthnum;i++)
		{
			if(finished[i]==0&&compare_work(work,i)==true){
				for(int j=0;j<resource;j++)
				{
					work[j]+=work[j]+allocation[i][j];
					finished[i]=true;
				}
			}
		}
	}
	int count=0;
	for(int i=0;i<pthnum;i++)
	{
		if(finished[i]==true)
			count++;
	}	
	if(count==pthnum)
		return SAFE;
	else
		return UNSAFE;
 }
void *request_thread(void *value){

	sem_wait(&mutex);
	int *value1=(int*)value;
	srand((unsigned int)time(NULL));
	int request[resource]={0};
	for(int i=0;i<resource;i++){
		request[i]=rand()%3;

	}
	usleep(1000000);
	cout<<"Thread "<<*value1<<" request "<<"A:"<<request[0]<<" B:"<<request[1]
	<<" C:"<<request[2]<<"\t";
	if(compare_need(request,*value1))
		{
			cout<<"request<=need"<<"\t";
			if(compare_available(request))
				{
					cout<<"request<=available"<<"\t";
					for(int i=0;i<resource;i++)
					{
						available[i]=available[i]-request[i];
						allocation[*value1][i]=allocation[*value1][i]+request[i];
						need[*value1][i]=need[*value1][i]-request[i];	
					}
					if(safety())
					{
						cout<<"Allocation done.Thread "<<*value1<<" got need."<<"SUCCESS!"<<endl;
					}
					else
					{
						for(int i=0;i<resource;i++)
						{
							available[i]=available[i]+request[i];
							allocation[*value1][i]=allocation[*value1][i]-request[i];
							need[*value1][i]=need[*value1][i]+request[i];	
						}
						cout<<"Not safe.Thread "<<*value1<<" must wait.NOT SUCCESS!"<<endl;
					}			
				}
			else
				cout<<"Thread must wait.There is no available resource.NOT SUCCESS!"<<endl;
		}
			
	else
		cout<<"Error!request>need.NOT SUCCESS!"<<endl;
	sem_post(&mutex);
}
int main()
{
	for(int i=0;i<pthnum;i++)
	{
		for(int j=0;j<resource;j++){
			need[i][j]=Max[i][j]-allocation[i][j];
		}
	}
	pthread_t pid[pthnum];
	pthread_attr_t attr[pthnum];
	int thread_count[pthnum]={0};
	sem_init(&mutex,0,1);
	for(int i=0;i<pthnum;i++)
		pthread_attr_init(&attr[i]);
	for(int i=0;i<pthnum;i++){
		thread_count[i]=i;
		//usleep(1000000);
		pthread_create(&pid[i],&attr[i],request_thread,(void*)&thread_count[i]);
	}
	pthread_join(pid[0],NULL);
	pthread_join(pid[1],NULL);
	pthread_join(pid[2],NULL);
	pthread_join(pid[3],NULL);
	pthread_join(pid[4],NULL);
	return 0;
}
















