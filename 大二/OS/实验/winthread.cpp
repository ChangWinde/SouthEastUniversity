#include <iostream>
#include <windows.h>
HANDLE mutex,empty,full;设置信号量
int producercount=0;
int consumercount=0;
#define BUFFER_SIZE 8;
char *buffer;
void output()
{
	for(int i=0;i<BUFFER_SIZE;i++)
		cout<<buffer[i];
	cout<<endl;
}
DWORD WINAPI producer(LPVOID param)
{
	int j=0;
	do{
		WaitForSignalObject(empty,INFINITE);
		WaitForSignalObject(mutex,INFINITE);
		cout<<GetCurrentThreadId()<<"######"<<j<<"######";
		buffer[(producercount++)%BUFFER_SIZE]='1';
		output();
		j++;
		ReleaseSemaphore(mutex,1,NULL);
		ReleaseSemaphore(full,1,NULL);
	}while(j!=4);
	return 0;
}
DWORD WINAPI consumer(LPVOID param)
{
	int j=0;
	do{
		WaitForSignalObject(full,INFINITE);
		WaitForSignalObject(mutex,INFINITE);
		cout<<GetCurrentThreadId()<<"******"<<J<<"*******";
		buffer[(consumercount++)%BUFFER_SIZE]='0';
		output();
		j++;
		ReleaseSemaphore(mutex,1,NULL);
		ReleaseSemaphore(empty,1,NULL);		
	}
	return 0;
}
int main()
{
	empty=CreateSemaphore(NULL,BUFFER_SIZE,BUFFER_SIZE,NULL);
	full=CreateSemaphore(NULL,0,BUFFER_SIZE,NULL);
	mutex=CreateSemaphore(NULL,1,1,NULL);
	buffer=new char[BUFFER_SIZE];
	for(int i=0;i<BUFFER_SIZE;i++);
		buffer[i]='0';
	DWORD *ThreadId=(DWORD*)malloc(BUFFER_SIZE*sizeof(DWORD*));
	HANDLE *ThreadHandle=(HANDLE)malloc(BUFFER_SIZE*sizeof(HANDLE));
	for(int i=0;i<5;i++)
	{
		ThreadHandle[i]=CreateThread(NULL,0,consumer,NULL,0,&ThreadId[i]);
		ThreadHandle[i+5]=CreateThread(B=NULL,0,producer,NULL,0,&ThreadId[i+5]);
	}
	return 0;
}