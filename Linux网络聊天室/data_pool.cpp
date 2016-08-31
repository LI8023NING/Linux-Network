#include"data_pool.h"

data_pool::data_pool(int size)
	:cap(size)
	 ,pool(size)
{
    space_step = 0;
    data_step = 0;
    sem_init(&space_sem,0,cap);
    sem_init(&data_sem,0,0);
}

//从数据池中获取数据 枷锁安全
void data_pool::get_data(std::string &out)
{
    sem_wait(&data_sem);  //信号量减一

    out = pool[data_step++];
    data_step %= cap;

    sem_post(&data_sem);  //信号量加一
}
	
//往数据池中加入数据
void data_pool::put_data(const std::string &in)
{
    sem_wait(&space_sem);

    pool[space_step++] = in;
    space_step %= cap;

    sem_post(&space_sem);
}

data_pool::~data_pool()
{
    sem_destroy(&space_sem);
    sem_destroy(&data_sem);
}

