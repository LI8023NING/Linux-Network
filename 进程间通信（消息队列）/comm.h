#pragma once

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
#include<errno.h>

#define _PROJ_NAME_ "/lining"
#define _PROJ_ID_ 0x6666
#define _SIZE_ 1024
#define _SERVER_TYPE_ 1
#define _CLIENT_TYPE_ 2

//自定义结构体
struct msgbuf
{
    long mtype;  //消息类型  必须>0
    char mtext;  //信息内容
};

//函数申明
//创建消息队列
int create_msg_queue();

//取得消息队列
int get_msg_queue();

//销毁消息队列
int destroy_msg_queue(int msg_id);

//接受
int recv_msg(int msg_id,int t,char* out);

//发送
int send_msg(int msg_id,int t,const char* msg);

