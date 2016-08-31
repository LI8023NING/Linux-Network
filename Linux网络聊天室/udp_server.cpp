#include"udp_server.h"

#define SIZE 1024

static void print_log(const char *str,const char *fun,size_t num)
{
    std::cout<<*str<<std::endl;
    std::cout<<*fun<<std::endl;
    std::cout<<num<<std::endl;
}


udp_server::udp_server(const std::string &_ip,const int &_port)
	:ip(_ip)
	 ,port(_port)
{
    init();
}

void udp_server::init()
{
    //套接字
    sock = socket(AF_INET,SOCK_DGRAM,0);
    if(sock < 0)
    {
	print_log("create socket erroe",__FUNCTION__,__LINE__);
	exit(1);
    }

    //本地协议族
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip.c_str());

    //绑定
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0)
    {
	print_log("bind socket erroe",__FUNCTION__,__LINE__);
	exit(2);
    }
}

//判断聊天的表中 有没有这个人 即就是在线用户
bool udp_server::is_friend_exist(const std::string &key)
{
    std::map<std::string,struct sockaddr_in>::iterator iter = online_user.find(key);
    if(iter == online_user.end())
    {
	return false;
    }
    else
    {
	return true;
    }
}

//添加在线用户  有客户端上来，就添加在线用户
int udp_server::add_friend(struct sockaddr_in &cli)
{
    std::string key = inet_ntoa(cli.sin_addr);  //ip 为 key
    std::cout<<"client ip is"<<key<<std::endl;  //上线客户端的ip 为 key

    if(!is_friend_exist(key)) //如果不存在  就插入客户端信息
    {
	online_user.insert(std::pair<std::string,struct sockaddr_in>(key,cli)); //一个IP 一个结构体
	return 0;
    }
    else
      return 1;   //如果存在返回1
}

//如果在线用户存在 就结束了他 map 中  erase 掉
int udp_server::clear_friend(struct sockaddr_in &cli)
{
    std::string key = inet_ntoa(cli.sin_addr);
    if(is_friend_exist(key))
    {
	online_user.erase(key);
	return 0;
    }
    else
      return 1;
}

//
int udp_server::recv_data(std::string &out) //jie shou
{
    char buf[SIZE];

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);

    ssize_t s = recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr*)&peer,&len);
    if(s > 0)
    {
	buf[s] = '\0';
	out = buf;
	add_friend(peer);
	pool.put_data(out);  //服务器接受到字符窜 还没有处理 只是字符串
    }
    else
    {
	print_log("recv_data erroe",__FUNCTION__,__LINE__);
    }
    return s;
}

//
int udp_server::send_data(const std::string &in,const struct sockaddr_in &cli,const socklen_t &len)
{
    ssize_t s = sendto(sock,in.c_str(),in.size(),0,(struct sockaddr*)&cli,len);
    if( s < 0)
    {
	print_log("sendto erroe",__FUNCTION__,__LINE__);
    }
    return s;
}

//
int udp_server::brocast_data()
{
    std::string in;
    pool.get_data(in); //得到数据放在in

    std::map<std::string,struct sockaddr_in>::iterator iter = online_user.begin();
    socklen_t len = 0;
    for(;iter != online_user.end();iter++)
    {
	len = sizeof(iter->second);  //iter->first 是 ip  iter->second 是一个客户端结构体 就是一个能表示唯一一个客户的
	send_data(in,iter->second,len);
    }
    return 0;
}

udp_server::~udp_server()
{
    if(sock > 0)
    {
	close(sock);
	sock = -1;
    }
}
