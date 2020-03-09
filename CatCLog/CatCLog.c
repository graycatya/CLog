// CLog.c: 定义应用程序的入口点。
//
#include "CatCLog.h"

int Log_Head(int grade, const char* msg, char** str)
{
    const char* Log_Grades[] = {"DEBUG", "INFO", "WARN", "ERROR", "ALARM", "FATAL"};
	time_t now;
    struct tm *tm_now ;
    time(&now) ;
    tm_now = localtime(&now);
    char* msgs = (char*)malloc(sizeof(char) * 60 + strlen(msg));
    sprintf(msgs, "%d-%02d-%02d %02d:%02d:%02d %s %s\n",
		tm_now->tm_year+1900, tm_now->tm_mon+1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, Log_Grades[grade], msg);
    *str = msgs;
    return strlen(msgs);
}


void* Log_Printf(void* arg)
{
    Log_Grade_Msg* msg = (Log_Grade_Msg*)arg;
    printf("%s", msg->msg);
    free(msg->msg);
    msg->msg = NULL;
    free(msg);
    msg=NULL;
}

void* Log_File(void* arg)
{
    Log_Grade_Msg* msg = (Log_Grade_Msg*)arg;
    FILE *fp = NULL;
    fp = fopen(msg->file_path, "a");
    fwrite( msg->msg, strlen(msg->msg), 1, fp);
    fclose(fp);
    free(msg->msg);
    msg->msg = NULL;
    free(msg->file_path);
    msg->file_path=NULL;
    free(msg);
    msg=NULL;
}

typedef struct Cat_CLog_Lock {
	pthread_mutex_t new_lock;
	pthread_mutex_t task_lock;
	pthread_cond_t 	task_cond;
} Cat_CLog_Lock;

static Cat_CLog_Lock Log_thread;

void Cat_CLog_Queue_Delete(Cat_CLog_Queue* q)
{
    if(q->front != NULL)
    {
        Cat_CLog_Node* temp = q->front;
        if(q->front == q->rear)
        {
            q->front = NULL;
            q->rear = NULL;
        } else {
            q->front = q->front->next;
        }
        free(temp);
        temp = NULL;
    } else {
        return;
    }
}

void* Task_thread(void* arg)
{
    Cat_CLog_Queue* Log_Singleton = (Cat_CLog_Queue*)arg;
    while(Log_Singleton->thread_start == 1)
    {
        pthread_mutex_lock(&Log_thread.task_lock);
        if(Cat_CLog_Queue_Empty(Log_Singleton) != 0)
        {
            Log_Singleton->front->thread_function(Log_Singleton->front->arg);
            Cat_CLog_Queue_Delete(Log_Singleton);
        } else {
            pthread_cond_wait(&Log_thread.task_cond, &Log_thread.task_lock);
        }
        pthread_mutex_unlock(&Log_thread.task_lock);
    }
    return Log_Singleton;
}

int Cat_CLog_Lock_Init( void )
{
    if(pthread_mutex_init(&Log_thread.new_lock, NULL) == 0 && \
        pthread_mutex_init(&Log_thread.task_lock, NULL) == 0 && \
            pthread_cond_init(&Log_thread.task_cond, NULL) == 0)
    {
        return 0;
    } 
    return -1;
}

int Cat_CLog_Lock_Destroy( void )
{
    if(pthread_mutex_destroy(&Log_thread.new_lock) == 0 && \
        pthread_mutex_destroy(&Log_thread.task_lock) == 0 && \
            pthread_cond_destroy(&Log_thread.task_cond) == 0)
    {
        return 0;
    }
    return -1;
}

Cat_CLog_Queue* Log_Singleton_Init( void )
{
    static Cat_CLog_Queue* Log_Singleton = NULL;
    if(Log_Singleton == NULL)
    {
        pthread_mutex_lock(&Log_thread.new_lock);
        if(Log_Singleton == NULL)
        {
            Log_Singleton = (Cat_CLog_Queue*)malloc(sizeof(Cat_CLog_Queue));
            Log_Singleton->front = Log_Singleton->rear = NULL;
            Log_Singleton->thread_start = 1;
            if(pthread_create(&Log_Singleton->thread, NULL, Task_thread, (void*)Log_Singleton) != 0)
            {
                printf("Log_Singleton_Init Thread init error!");
            }
        }
        pthread_mutex_unlock(&Log_thread.new_lock);
    }
    return Log_Singleton;
}

int Cat_CLog_Queue_Empty(Cat_CLog_Queue* q)
{
    if(q->front == NULL)
    {
        return 0;
    }
    return -1;
}

void Cat_CLog_Queue_Insert(Cat_CLog_Queue* q, void*(*thread_function)(void* arg), void* arg)
{
    Cat_CLog_Node* temp = (Cat_CLog_Node*)malloc(sizeof(Cat_CLog_Node));
    if(temp == NULL)
    {
        printf("Cat_CLog_Queue_Insert temp malloc error!");
        return;
    } 
    temp->thread_function = thread_function;
    temp->arg = arg;
    temp->next = NULL;
    pthread_mutex_lock(&Log_thread.task_lock);
    if(q->front == NULL)
    {
        
        q->front = temp;
        q->rear = temp;
    } else {
        q->rear->next = temp;
        q->rear = temp;
    }
    pthread_mutex_unlock(&Log_thread.task_lock);
    pthread_cond_signal(&Log_thread.task_cond);
}

int Log_Singleton_Delete(Cat_CLog_Queue* q)
{
    if(q != NULL)
    {
        {
            pthread_mutex_lock(&Log_thread.new_lock);
            if(q->thread_start == 1)
            {
                while(Cat_CLog_Queue_Empty(q) != 0)
                {
                    pthread_cond_signal(&Log_thread.task_cond);
                }
                q->thread_start = 0;
                pthread_mutex_unlock(&Log_thread.new_lock);
            } else {
                return -1;
            }
            pthread_cond_signal(&Log_thread.task_cond);
            pthread_join(q->thread, NULL);
        }
        if(q->front == NULL)
        {
            free(q);
            q = NULL;
        } else {
            return -1;
        }
    }
    return 0;
}

void Log_Out(int grade, const char* msg)
{
	Log_Grade_Msg* log_msg = (Log_Grade_Msg*)malloc(sizeof(Log_Grade_Msg));
    //log_msg->msg = (char*)malloc(sizeof(char)*2048);
    log_msg->msg = NULL;
	Log_Head(grade, msg, &(log_msg->msg));
	//strcpy(log_msg->msg, str);
	Cat_CLog_Queue_Insert(Log_Singleton_Init(), Log_Printf, (void*)log_msg);
}

void Log_OutFile(int grade,const char* filename, const char* msg)
{
	Log_Grade_Msg* log_msg = (Log_Grade_Msg*)malloc(sizeof(Log_Grade_Msg));
    //log_msg->msg = (char*)malloc(sizeof(char)*2048);
    log_msg->msg = NULL;
    log_msg->file_path = NULL;
    log_msg->file_path = (char*)malloc(strlen(filename)+1);
    memcpy(log_msg->file_path, filename, strlen(filename)+1);
    //strcpy(log_msg->file_path, filename);
	Log_Head(grade, msg, &(log_msg->msg));
	//strcpy(log_msg->msg, str);
	Cat_CLog_Queue_Insert(Log_Singleton_Init(), Log_File, (void*)log_msg);
}