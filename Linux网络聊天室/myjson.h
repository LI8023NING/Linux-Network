#pragma once

#include<iostream>
#include<string>
#include"json/json.h"

class myjson
{
    public:
	static void serialize(Json::Value &val,std::string &out);
	static void unserialize(std::string &in,Json::Value &val);
};
