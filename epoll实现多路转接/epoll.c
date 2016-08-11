#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>


void usage(const char* argv)
{ 
  printf("%s:[ip][port]\n",argv);
}

void set_nonblock(int fd)
{
    int fl = fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,fl | O_NONBLOCK);
}

int startup(char* _ip,int _port)  //创建一个套接字，绑定，检测服务器
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
  local.sin_port=htons(_port);
  local.sin_family=AF_INET;
  local.sin_addr.s_addr=inet_addr(_ip);

  //3.bind（）绑定
  if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0) 
  {
      perror("bind");
      exit(3);
  }
  //4.listen（）监听 检测服务器
  if(listen(sock,5)<0)
  {
      perror("listen");
      exit(4);
  }
  return sock;    //这样的套接字返回
}

int main(int argc,char *argv[])                         
{
    if(argc!=3)     //检测参数个数是否正确
    {
        usage(argv[0]);
        exit(1);
    }

    int listen_sock=startup(argv[1],atoi(argv[2]));      //创建一个绑定了本地 ip 和端口号的套接字描述符


    //1.创建epoll    
    int epfd = epoll_create(256);    //可处理的最大句柄数256个
    if(epfd < 0)
    {
	perror("epoll_create");
	exit(5);
    }

    struct epoll_event _ev;       //epoll结构填充 
    _ev.events = EPOLLIN;         //初始关心事件为读
    _ev.data.fd = listen_sock;   

    //2.托管
    epoll_ctl(epfd,EPOLL_CTL_ADD,listen_sock,&_ev);  //将listen sock添加到epfd中，关心读事件

    struct epoll_event revs[64];

    int timeout = -1;
    int num = 0;
    int done = 0;

    while(!done)
    {
	//epoll_wait()相当于在检测事件
        switch((num = epoll_wait(epfd,revs,64,timeout)))  //返回需要处理的事件数目  64表示 事件有多大
        {
            case 0:                  //返回0 ，表示监听超时
                printf("timeout\n");
                break;
            case -1:                 //出错
                perror("epoll_wait");
                break;
            default:                 //大于零 即就是返回了需要处理事件的数目
              {
		struct sockaddr_in peer;
		socklen_t len = sizeof(peer);

		int i;
                for(i=0;i < num;i++)
                {
		    int rsock = revs[i].data.fd; //准确获取哪个事件的描述符
                    if(rsock == listen_sock && (revs[i].events) && EPOLLIN) //如果是初始的 就接受，建立链接
                    {
                        int new_fd = accept(listen_sock,(struct sockaddr*)&peer,&len);  

                    	if(new_fd > 0)
                    	{
                             printf("get a new client:%s:%d\n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));

			     set_nonblock(new_fd);
			     _ev.events = EPOLLIN | EPOLLET;
			     _ev.data.fd = new_fd;
			     epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&_ev);    //二次托管

                        }
		    }
		    else // 接下来对num - 1 个事件处理
		    {
			if(revs[i].events & EPOLLIN)
			{
			    char buf[1024];
			    ssize_t _s = read(rsock,buf,sizeof(buf)-1);
			    if(_s > 0)
			    {
				buf[_s] = '\0';
				printf("client:%s\n",buf);

			        _ev.events = EPOLLOUT | EPOLLET;
			        _ev.data.fd = rsock;
			        epoll_ctl(epfd,EPOLL_CTL_DEL,rsock,&_ev);    //二次托管
			    }
			    else if(_s == 0)  //client:close
			    {
				printf("client:%d close\n",rsock);
				epoll_ctl(epfd,EPOLL_CTL_DEL,rsock,NULL);

				close(rsock);
			    }
			    else
			    {
				perror("read");
			    }
			}
			else if(revs[i].events & EPOLLOUT)
			{
			    const char *msg = "HTTP/1.0.200 OK\r\n\r\n<html><h2>李宁爱张宁！</h2></html>\r\n";
			    write(rsock,msg,strlen(msg));
			    epoll_ctl(epfd,EPOLL_CTL_DEL,rsock,NULL);
			    close(rsock);
			}
			else
			{}
		    }
		}
	      }
	      break;
	}
    }
    return 0;
}
