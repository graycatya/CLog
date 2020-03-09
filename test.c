#include"CatCLog/CatCLog.h"

#include <stdio.h>
#include <stdlib.h>


void* myprintf(void* arg)
{
	int* i = (int*)arg;
	printf("Hello World! %d\n", i);
}

void* myprintfs(void* arg)
{
	Cat_CLog_Queue* Log = Log_Singleton_Init();
	for(int i = 0; i < 100; i++)
	{
		Log_OutFile(_WARN_, "./log.log", "warn");
	}
	Log_Singleton_Delete(Log);
}

int main()
{
	
	pthread_t test0;
	Cat_CLog_Lock_Init();
	Cat_CLog_Queue* Log = Log_Singleton_Init();
	pthread_create(&test0, NULL, myprintfs, NULL);
	for(int i = 0; i < 100; i++)
	{
		Log_Out(_DEBUG_, "debug");
		//FATAL_PRINTF(i);
	}
	pthread_join(test0, NULL);
	Log_Singleton_Delete(Log);
	Cat_CLog_Lock_Destroy();
	return 0;
}