#ifndef BIG_DATA_H
#define BIG_DATA_H

#include<string>
#include<iostream>
using namespace std;

#define UN_INT 0xcccccccccccccccc    //初始化值
#define MAX_INT64 0x7fffffffffffffff //64bit为可存最大的数（一个long long）
#define MIN_INT64 0x8000000000000000 //64bit为可存最小的数

typedef long long INT64; 

class BigData
{
public:
	BigData(INT64 data = UN_INT); //缺省构造 初始值给一个long long 类型的data
	BigData(const char *pData);   //构造 传参过来的是一个字符串

	BigData operator+(BigData& bigData);
	BigData operator-(const BigData& bigData);
	BigData operator*(const BigData& bigData);
	BigData operator/(const BigData& bigData);
	BigData operator%(const BigData& bigData);


//================================================
	bool operator<(const BigData& bigData);
	bool operator>(const BigData& bigData);
	bool operator==(const BigData& bigData);

//输入输出
	friend std::istream& operator>>(std::istream& _cin,BigData bigData);
	friend std::ostream& operator<<(std::ostream& _cout,const BigData& bigData);

	bool IsINT64Owerflow() const;


private:
	std::string Add(std::string left,std::string right);
	std::string Sub(std::string left,std::string right);
	std::string Mul(std::string left,std::string right);
	std::string Div(std::string left,std::string right);

	void INT64ToString();

	bool IsLeftStrBig(char *pLeft,size_t LSize,char *pRight,size_t RSize);
	char SubLoop(char *pLeft,size_t LSize,char *pRight,size_t RSize);
private:
	long long m_llValue;    //可以装在longlong中
	std::string m_strData;  //否则装在字符串中  //也可全转在字符串中 但是效率低
};

#endif