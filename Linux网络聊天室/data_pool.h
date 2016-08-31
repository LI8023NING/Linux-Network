#pragma once

#include<iostream>
#include<vector>
#include<string>
#include<semaphore.h>

class data_pool   //数据池类
{
    public:
	data_pool(int size = 128);
	void get_data(std::string& out);
	void put_data(const std::string& in);
	~data_pool();
    private:
	std::vector<std::string> pool; //元素都是字符串
	int cap;  //传说中的容量
	int space_step;
	int data_step;
	sem_t space_sem;   //传说中的两个信号量
	sem_t data_sem;
};
