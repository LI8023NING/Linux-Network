#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define BUFFSIZE 4096

int main()
{
    int n;
    char buf[BUFFSIZE];

    while((n = read(0,buf,BUFFSIZE)) > 0) //0:STDIN_FILENO
    {
     	if(write(1,buf,n) != n)          //1:STDOUT_FILENO
	{
	    printf("写出错\n");
	    return -1;
	}
    }

    if(n < 0)
    {
	printf("读出错\n");
    }
    return 0;
}


/* ./copy < 被复制的普通文件 >  复制到哪个文件
 */
