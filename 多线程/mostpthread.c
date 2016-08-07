#include<stdio.h>
#include<pthread.h>

void *foo(void* arg)
{
    ;
}

int main() 
{ 
    int i = 0; 
    pthread_t thread; 
    while (1) 
    { 
	if (pthread_create(&thread, NULL, foo, NULL) != 0) 
	  return; 
	i ++; 
	printf("i = %d\n", i); 
    } 
    return 0;
}
