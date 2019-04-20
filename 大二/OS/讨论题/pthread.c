#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#define BUFFER_SIZE 8
char *buffer;
sem_t mutex,empty,full;//mutex为互斥信号量，用于互斥的访问buffer；empty和full为资源信号量，分别用于记录缓冲区空和满的数量
int producer_count,consumer_count;//生产者和消费者在数组中的下标
void output()
{
	for(int i=0;i<BUFFER_SIZE;i++)
	{
		printf("%c%s",buffer[i]," ");
	}
	printf("\n");
}
void *producer(void *ptr)
{
	int j=0;
	do{
		sem_wait(&empty);//buffer有空余，可以生产，并减一
		sem_wait(&mutex);//互斥访问，只能一个线程消费
		printf("%lu%s%d%s\n",pthread_self(),"######",j,"######");
		buffer[(producer_count++)%BUFFER_SIZE]='1';//生产者线程赋值为1；
		output();
		j++;
		sem_post(&mutex);
		sem_post(&full);//生产完毕增加一个可消费的量
	}while(j!=4);//每个线程可以做4次
}
void *consumer(void *ptr)
{
	int j=0;
	do{
		sem_wait(&full);//可以消费的量减一
		sem_wait(&mutex);
		printf("%lu%s%d%s\n",pthread_self(),"******",j,"******");
		buffer[(consumer_count++)%BUFFER_SIZE]='0';//消费者线程赋值为0；
		output();
		j++;
		sem_post(&full);
		sem_post(&empty);
	}while(j!=4);
}
int main()
{
	producer_count=0;
	consumer_count=0;
	buffer=(char*)malloc(BUFFER_SIZE*sizeof(char));
	for(int i=0;i<BUFFER_SIZE;i++)
		buffer[i]='0';
	//信号量赋初值
	sem_init(&mutex,0,1);
	sem_init(&empty,0,BUFFER_SIZE);
	sem_init(&full,0,0);//初始化信号量
	//多线程
	pthread_t tid[10];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	for(int i=0;i<5;i++)
	{
		pthread_create(&tid[i],&attr,producer,NULL);
		pthread_create(&tid[i+5],&attr,consumer,NULL);		
	}
	for(int j=0;j<10;j++)
	{
			pthread_join(&tid[j],NULL);
	}
	return 0;
}