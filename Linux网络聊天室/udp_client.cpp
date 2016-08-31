#include"udp_client.h"

#define SIZE 1024
void print_log(const char *str,const char *fun,size_t num)
{
    std:: cout<<"出错了"<<std::endl;
    std:: cout<<*str<<std::endl;
    std:: cout<<*fun<<std::endl;
    std:: cout<<num<<std::endl;
}


udp_client::udp_client(const std::string &_ip,const int &_port)
	:remote_ip(_ip)   //ip地址
	 ,remote_port(_port) //端口号
{
    init();    //初始化
}

void udp_client::init()
{
    sock = socket(AF_INET,SOCK_DGRAM,0); //客户端有意个文件描述符
    if(sock < 0)
    {
	print_log("create socket erroe",__FUNCTION__,__LINE__);
	exit(1);
    }
}

//客户端从服务器收收据 收来的数据放到out中
int udp_client::recv_data(std::string &out) //jie shou
{
    char buf[SIZE];

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

    ssize_t s = recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&peer,&len); //接受 通过描述符
    if(s > 0)
    {
	buf[s] = '\0';
	out = buf;
    }
    else
    {
	print_log("recv_data erroe",__FUNCTION__,__LINE__);
    }
    return s;
}


//发送数据给服务器，将中的数据发送走 到sock中
int udp_client::send_data(const std::string &in)
{
    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_port = htons(remote_port);
    remote.sin_addr.s_addr = inet_addr(remote_ip.c_str());
    socklen_t len = sizeof(remote);

    ssize_t s = sendto(sock,in.c_str(),in.size(),0,(struct sockaddr*)&remote,len);
    if( s < 0)
    {
	print_log("sendto erroe",__FUNCTION__,__LINE__);
    }
    return s;
}

udp_client::~udp_client()
{
    if(sock > 0)
    {
	close(sock);
	sock = -1;
    }
}
