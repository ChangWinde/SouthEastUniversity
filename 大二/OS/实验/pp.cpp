#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h> 
#define BUFFER_SIZE 8//缓冲区大小为8
char *buffer;
sem_t mutex,empty,full;//三个信号量，互斥信号量mutex，技术信号量empty和full
int x,y;//生产者和消费者在buffer中下标
void output()//输出buffer数组
{
    int i;
    for(i=0;i<BUFFER_SIZE;i++)
    {
        printf("%c",buffer[i]);
        printf(" ");
    }
    printf("\n");
}
void *produce(void *ptr)//生产者函数
{
    int j;
    j=0;    
    do
    {
        sem_wait(&empty);//buffer有空余部分，可以生产，并减一
        sem_wait(&mutex);//形成互斥访问，只能一个线程生产
        printf("%lu%s%d%s\n",pthread_self(),"++++",j,"++++ ");//输出当前线程的id号，以及正在执行的次数
        
        buffer[(x++)%BUFFER_SIZE]='1';//生产就赋值1
        output();//输出buffer
        j++;
        sem_post(&mutex);//取消互斥
        sem_post(&full);//生成完毕，增加一个可以消费量。
    }while (j!=4);//每个线程可以做4次
}
void *consume(void *ptr)//消费者函数
{
    int j;
    j=0;    
    do
    {
        sem_wait(&full);//可以消费的量减一
        sem_wait(&mutex);//互斥访问，只能一个线程消费
        printf("%lu%s%d%s\n",pthread_self(),"----",j,"---- ");
        buffer[(y++)%BUFFER_SIZE]='0';//消费时，赋值为0
                output();//输出buffer值

        j++;
        sem_post(&mutex);//取消互斥，其他线程可以消费
        sem_post(&empty);//空余量加一
    }while (j!=4);//每个线程可以消费4次
}

int main()
{
    int i;
    x=0;
    y=0;
    buffer=(char*)malloc(BUFFER_SIZE*sizeof(char*));
    for(i=0;i<BUFFER_SIZE;i++)//初始化buffer数组，默认为0
    {
        buffer[i]='0';
    }
    //semaphore 
    sem_init(&mutex,1,1);//初始化互斥信号量mutex为1
    sem_init(&empty,0,BUFFER_SIZE);//初始化计数信号量empty为BUFFER_SIZE
    sem_init(&full,0,0);//初始化计数信号量full为0，pshared为0表示为进程内的线程共享
    //multipthread
    pthread_t tid[10];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //创建5个生产者线程和5个消费者线程
    for(i=0;i<5;i++)
    {
        pthread_create(&tid[i],&attr,consume,NULL);
        pthread_create(&tid[i+5],&attr,produce,NULL);       
    }   
//让每个线程在主线程main执行前全部执行完毕。
    for(i=0;i<10;i++)
    {
        pthread_join(tid[i],NULL);
    }
    return 0;
}
