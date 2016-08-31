#pragma once

#include<iostream>
#include<string>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
//#include"common.h"
#include"udp_data.h"

class udp_client
{
    public:
	udp_client(const std::string &_ip = "127.0.0.1",\
		    const int &_port = 9999);
	void init();
	int recv_data(std::string &out);
	int send_data(const std::string &in);
	~udp_client();

    private:
	int sock;
	std::string remote_ip;
	int remote_port;
};
