#include<stdlib.h>
#include<stdlib.h>
#include<pthread.h>

void *thread_run(void *str)
{
    printf("%s\n",(char*)str);
    printf("次线程正在运作，请稍后...\n");
    sleep(5);
    printf("次线程运作结束..\n");
    pthread_exit((void*)2); 
}

int main()
{
    pthread_t id;
    int ret = pthread_create(&id,NULL,thread_run,"创建了一个线程...");
    if(ret !=  0)
    {
	printf("创建线程错误...\n");
	return ret;
    }

    //sleep(6);
    pthread_cancel(id);   //取消线程
    int re = 0;
   // sleep(1);

    if(0 == pthread_join(id,NULL)) //等待 id这个线程结束，等待成功返回零，失败返回错误号
    {
	printf("线程等待成功...,并且资源回收完成...(线程合并)\n");
	re = 0;
    }

    else
    {
	printf("线程等待失败...\n");
	re = 1;
    }

    return re;
}
