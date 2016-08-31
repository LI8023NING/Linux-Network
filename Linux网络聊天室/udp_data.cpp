#include"udp_data.h"

udp_data::udp_data()
{}

void udp_data::to_string(std::string &out)
{
    Json::Value val;
    val["nick_name"] = nick_name;
    val["school"] = school;
    val["msg"] = msg;
    val["cmd"] = cmd;

    myjson::serialize(val,out);  //将数据打包到out中
}

void udp_data::to_value(std::string &in)
{
    Json::Value val;
    myjson::unserialize(in,val); //将IN中的数据分析出来到val
    
    nick_name = val["nick_name"].asString();
    school = val["school"].asString();
    msg = val["msg"].asString();
    cmd = val["cmd"].asString();
}

udp_data::~udp_data()
{}
