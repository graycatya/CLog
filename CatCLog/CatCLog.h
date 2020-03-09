// CLog.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define _DEBUG_ 0
#define _INFO_  1
#define _WARN_  2
#define _ERROR_ 3
#define _ALARM_ 4
#define _FATAL_ 5


typedef struct Log_Grade_Msg {
	char* file_path;
	char* msg;
} Log_Grade_Msg;

typedef struct Cat_CLog_Node {
	void* (*thread_function)(void* arg);	//任务线程
	void* arg;								//任务数据
	struct Cat_CLog_Node* next;				//下一个节点
} Cat_CLog_Node;

typedef struct Cat_CLog_Queue {
	pthread_t thread;	  //下一个线程
	int thread_start;     //线程启动标志位
	Cat_CLog_Node* front; //前一个节点
	Cat_CLog_Node* rear;  //下一个节点
} Cat_CLog_Queue;

int Cat_CLog_Lock_Init( void );
int Cat_CLog_Lock_Destroy( void );
Cat_CLog_Queue* Log_Singleton_Init( void );
int Cat_CLog_Queue_Empty(Cat_CLog_Queue* q);
void Cat_CLog_Queue_Insert(Cat_CLog_Queue* q, void*(*thread_function)(void* arg), void* arg);
int Log_Singleton_Delete(Cat_CLog_Queue* log);

int Log_Head(int grade, const char* msg, char** str);
//void* Log_Printf(void* arg);
//void* Log_File(void* arg);
void Log_Out(int grade, const char* msg);
void Log_OutFile(int grade,const char* filename, const char* msg);


// TODO: 在此处引用程序需要的其他标头。
