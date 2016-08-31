#include"myjson.h"

void myjson::serialize(Json::Value &val,std::string &out)
{
#ifdef _FAST_
    Json::FastWriter _w;
#else
    Json::StyledWriter _w;
#endif
    out = _w.write(val);
}

void myjson::unserialize(std::string &in,Json::Value &val)
{
    Json::Reader _r;
    _r.parse(in,val,false);   
}
