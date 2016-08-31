#pragma once

#include<iostream>
#include<string>
#include "myjson.h"

class udp_data
{
    public:
	udp_data();
	void to_string(std::string &out);
	void to_value(std::string &in);
	~udp_data();

    public:
	std::string nick_name;
	std::string school;
	std::string msg;
	std::string cmd;
};
