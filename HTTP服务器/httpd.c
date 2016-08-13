#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SIZE 1024

//1.可能存在问题
void usage(const char* argv)
{ 
  printf("%s:[ip][port]\n",argv);
}


//创建一个套接字，绑定，检测服务器
static int start_up(char* ip,int port)  
{
  //sock
  //1.创建套接字
  int sock=socket(AF_INET,SOCK_STREAM,0);   
  if(sock<0)
  {
      perror("sock");
      exit(2);
  }
  
  int opt = 1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
  
  //2.填充本地 sockaddr_in 结构体（设置本地的IP地址和端口）
  struct sockaddr_in local;       
  local.sin_port=htons(port);
  local.sin_family=AF_INET;
  local.sin_addr.s_addr=inet_addr(ip);

  //3.bind（）绑定
  if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0) 
  {
      perror("bind");
      exit(3);
  }
  //4.listen（）监听 检测服务器
  if(listen(sock,5)<0)
  {
      perror("sock");
      exit(4);
  }
  return sock;    //这样的套接字返回
}

//主函数
int main(int argc,char *argv[])
{
    if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}
	
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

	int listen_sock = start_up(argv[1],atoi(argv[2]));
	int done = 0;

	while(!done)
    {
		//接收客户端上来的链接
		int new_fd = accept(listen_sock,(struct sockaddr*)&peer,&len);

		if(new_fd > 0)
		{
			pthread_t id;
			//建一个新线程... 
			pthread_create(&id,NULL,handler_data,(void*)new_fd);
			pthread_detach(id);    //每建立一个线程，将其分离，不会阻塞
		}
    }
    return 0;
}
