poll函数

INFTIM	永远等待
0	立即返回，不阻塞进程
＞0	等待指定数目的毫秒数
　　1.#include <poll.h>
　　3.int poll(struct pollfd *fdarray, unsigned long nfds, int timeout);
　　4.返回：就绪描述字的个数，0－超时，-1－出错

　　第一个参数是指向一个结构数组第一个元素的指针。每个数组元素都是一个pollfd结构，用于指定测试某个给定描述字fd的条件。

　　struct pollfd{

　　int fd;              //descriptor to check

　　short events;    //events of interest on fd

　　short revents;   //events that occurred on fd

　　};
一个使用poll的网络程序例子:

//TCP服务器的服务端程序
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>　
#include <string.h>
#include <errno.h>
#include <poll.h>   //for poll

#define LISTENQ 1024
#define MAXLINE 1024
#define OPEN_MAX 50000

#ifndef INFTIM 
#define INFTIM -1 
#endif             

int start_up(char* ip,int port)  //创建一个套接字，绑定，检测服务器
{
  //sock
  //1.创建套接字
  int sock=socket(AF_INET,SOCK_STREAM,0);   
  if(sock<0)
  {
      perror("sock");
      exit(0);
  }
  
  //2.填充本地 sockaddr_in 结构体（设置本地的IP地址和端口）
  struct sockaddr_in local;       
  local.sin_port=htons(port);
  local.sin_family=AF_INET;
  local.sin_addr.s_addr=inet_addr(ip);

  //3.bind（）绑定
  if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0) 
  {
      perror("bind");
      exit(1);
  }
  //4.listen（）监听 检测服务器
  if(listen(sock,back_log)<0)
  {
      perror("sock");
      exit(1);
  }
  return sock;    //这样的套接字返回
}
	
int main(int argc, char *argv[])
{
　　int i, maxi, connfd, sockfd;
　　int nready;
　　ssize_t n;
　　socklen_t clilen;

　　struct sockaddr_in servaddr;
    socklen_t len=sizeof(servaddr); 

　  char buf[BUFSIZ];

　　    struct pollfd client[OPEN_MAX]; // 用于poll函数第一个参数的数组,存放每次的文件描述符个数
　　    if( argc != 3 )
		{
　　        printf("Please input %s <hostname>\n", argv[0]);
	     	exit(2);
		}

		int listenfd=start_up(argv[1],argv[2]);      //创建一个绑定了本地 ip 和端口号的套接字描述符
		
　　    client[0].fd = listenfd;         //将数组中的第一个元素设置成监听描述字
　　    client[0].events = POLLIN;       //将测试条件设置成普通或优先级带数据可读(感兴趣的事件读、写、出错)，此处书中为POLLRDNORM，*/
		client[0].revents = 0;           //真正发生的事件

　　    for(i = 1;i < OPEN_MAX; ++i)     //数组中的其它元素将暂时设置成不可用
		{
			client[i].fd = -1;
		}
        
　　    maxi = 0;
　      while(1)
　　    {
　　       nready = poll(client, maxi+1,INFTIM);          //将进程阻塞在poll上
　　       if( client[0].revents & POLLIN)                //先测试监听描述字
　　       {
				connfd = accept(listenfd,(struct sockaddr*)&servaddr, &clilen);

　　            for(i = 1; i < OPEN_MAX; ++i)
				{
					if( client[i].fd < 0 )
　　                {
　                     client[i].fd = connfd;      //将新连接加入到测试数组中
　　                   client[i].events = POLLIN;  //POLLRDNORM; 测试条件普通数据可读
　                     break;
　　                }
　　            	if( i == OPEN_MAX )
　              	{
　　               	 	printf("too many clients"); //连接的客户端太多了，都达到最大值了
　                  	exit(1);
　　            	}

　　            	if( i > maxi )
　　                	maxi = i;        //maxi记录的是数组元素的个数

　　            	if( --nready <= 0 )
　　               	continue;            //如果没有可读的描述符了，就重新监听连接
				}
　　        }

　　        for(i = 1; i <= maxi; i++)  //测试除监听描述字以后的其它连接描述字
　　        {
				if( (sockfd = client[i].fd) < 0) //如果当前描述字不可用，就测试下一个
　　                continue;

　　            if(client[i].revents & (POLLIN | POLLERR))//如果当前描述字返回的是普通数据可读或出错条件
　　            {
　                if( (n = read(sockfd, buf, MAXLINE)) < 0) //从套接口中读数据
　　               {
　　                    if( errno == ECONNRESET) //如果连接断开，就关闭连接，并设当前描述符不可用
　　                    {
　　                       close(sockfd);
　　                       client[i].fd = -1;
　　                    }
　　                    else
　　                        perror("read error");
　　                }
　               else if(n == 0) //如果数据读取完毕，关闭连接，设置当前描述符不可用
　　                {
　　                    close(sockfd);
　　                    client[i].fd = -1;
　　                }
　　                else
　　                    write(sockfd, buf, n); //打印数据
　　             if(--nready <= 0)
　　                 break;
　　            }
　　        }
　　   }
　　   exit(0);
　　}