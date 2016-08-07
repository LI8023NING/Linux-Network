#include<stdio.h>
#include<unistd.h>
#include<error.h>
#include<string.h>

int main()
{
    int _pipe[2];
    int ret = pipe(_pipe);  //类似两个文件指针，指向两个流，0是读，1是写
    if(ret == -1)
    {
	printf("创建管道失败，错误码是：%d,%d\n",error,strerror(error));
	return 1;
    }

    pid_t id = fork();   //创建子进程
  
    if(id < 0)
    {
	printf("子进程错误\n");
	return 2;
    }

    else if(id == 0)  //child  子进程相关操作 子进程写
    {
	close(_pipe[0]);  //子进程关闭了读 自然就选择了写
	int i = 0;
	char* mesg = NULL;
	while(i < 20)
	{
	    mesg = "我是子进程，我在写，我会被父进程读出来......";
	    write(_pipe[1],mesg,strlen(mesg));
	    sleep(2);  //每写一次延时2秒
	    i++;
	}

	//写完之后关闭写端，但是读端继续读
	//////////////////////////////
	close(_pipe[1]);
	//////////////////////////////
    }
    else   //父进程相关操作 父进程读
    {
	close(_pipe[1]);  //关闭了写，也就只能读了
	char _mesg[1024];
	int j = 0;
	int i = 0;
	while(j < 200)
	{
	    memset(_mesg,'\0',sizeof(_mesg));
	    int ret = read(_pipe[0],_mesg,sizeof(_mesg));
	    printf("%s,%d,%d\n",_mesg,ret,i++);
	    j++;
	}

	//waitpid
	////////////////////////////
	if(waitpid(id,NULL,0)<0)
	{
	    return 3;
	}
	///////////////////////////
    }
    return 0;
}
