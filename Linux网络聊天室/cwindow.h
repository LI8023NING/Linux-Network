#pragma once

#include<iostream>
#include<string>
#include<ncurses.h>
#include<strstream>

class cwindow
{
    public:
	cwindow();
	void create_header();   //创建头
	void create_output();   //创建输出框 
	void create_flist();    //在线好友列表
	void create_input();    //输入框

	void myflush(WINDOW *_win); //刷新窗口
	void clean_win_line(WINDOW *win,int begin,int line);  //清除某一个小窗口的几行

	void put_str_to_win(WINDOW *win,int _y,int _x,std::string &str); //给固定串口的固定坐标处写字符


	~cwindow();
    public:
	WINDOW *header;
	WINDOW *output;
	WINDOW *flist;
	WINDOW *input;
};
