#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define pthnum=5;
#define resource=3;
#define SAFE=true;
#define UNSAFE=false;

int max[pthnum][resource]={
	{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}
};
int allocation[pthnum][resource]={
	{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}
};
int available[resource]={3,3,2};
int finished[pthnum]={0};
int need[pthnum][resource];
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
	for(j=0;j<resource;j++)
	{
		work[j]=available[j];
	}
	for(int k=0;k<pthnum;k++){
		for(int i=0;i<pthnum;i++)
		{
			if(finished[i]==0&&compare_work(work,i)==true){
				for(int j=0,j<resource;j++)
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
void request_thread(int value){
	srand((unsigned))time(NULL);
	int request[pthnum][resource]={0};
	for(int i=0;i<resource;i++){
		request[value][i]=rand()%4;
	}
	cout<<"Thread "<<"value"<<"request "<<"A:"request[0]<<" B:"<<request[1]
	<<" C:"<<request[2]<<"\t";
	sem_wait(&mutex);
	if(compare_need(*request,value))
		{
			cout<<"request<=need"<<"\t";
			if(compare_available(*request))
				{
					cout<<"request<=available"<<endl;
					for(int i=0;i<resource;i++)
					{
						available[i]=available[i]-request[value][i];
						allocation[value][i]=allocation[value][i]+request[value][i];
						need[value][i]=need[value][i]-request[value][i];	
					}
					if(safety())
					{
						cout<<"Allocation done.Thread "<<"value"<<"got need."<<endl;
					}
					else
					{
						for(int i=0;i<resource;i++)
						{
							available[i]=available[i]+request[value][i];
							allocation[value][i]=allocation[value][i]-request[value][i];
							need[value][i]=need[value][i]+request[value][i];	
						}
						cout<<"Not safe.Thread "<<"value"<<" must wait.";
					}			
				}
			else
				cout<<"Thread must wait.There is no available resource."<<endl;
		}
			
	else
		cout<<"error!request>need"<<"\t";
	sem_post(&mutex);
}
int main()
{
	for(int i=0;i<pthnumber;i++)
	{
		for(int j=0;j<resource;j++){
			need[i][j]=max[i][j]-allocation[i][j];
		}
	}
	pthread_t pid[pthnumber];
	pthread_attr_t attr[pthnumber];

	sem_ini(&mutex,0,1);
	for(int i=0;i<pthnumber;i++)
		pthread_attr_init(&attr[i]);
	for(int i=0;i<pthnumber;i++){
		thread_count[i]=i;
		pthread_create(&pid[i],&attr[i],request_thread,&thread_count[i]);
	}
	pthread_join(ptid[0],NULL);
	pthread_join(ptid[1],NULL);
	pthread_join(ptid[2],NULL);
	pthread_join(ptid[3],NULL);
	pthread_join(ptid[4],NULL);
	return 0;
}
















