#include "BigData.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <cassert>

//构造函数  初始化工作 整数
BigData::BigData(INT64 data)
	: m_llValue(data)  //默认 UN_INT
	, m_strData("")
{
	INT64ToString();  //将 longlong 型装到string 以备long long 类型数据和string类数据相运算
}
 
//接受字符处理  构造函数  初始化工作
BigData::BigData(const char *_pData)
{
	// "12345789"  "1234567" "+" "12457aaa123" "000001234567"
	// "a23456789" 
	assert(NULL != _pData);

	char cSybom = _pData[0];   //符号为 预留
	char* pData = (char*)_pData; //pData指向传来的字符串；
	//
	if ('+' == cSybom || '-' == cSybom)
	{
		pData++;
	}
	else if (*pData >= '0' && *pData <= '9')
	{
		cSybom = '+';
	}
	else
	{
		m_llValue = 0;
		m_strData = "0";
		return;
	}

	// 去掉前置0
	while('0' == *pData)
		pData++;

	// "12457aaa123"  这里的pdata已经剔除前置0和符号位
	m_strData.resize(strlen(pData)+1);  //为什么加1 符号位还有一位
	m_llValue = 0;
	m_strData[0] = cSybom;
	int iCount = 1;
	while(pData)
	{
		if (*pData >= '0' && *pData <= '9') //遇到非法字符 直接跳出
		{
			m_llValue = m_llValue*10 + *pData - '0';//不溢出就用 溢出就用字符的
			m_strData[iCount++] = *pData++;
		}
		else
		{
			break;
		}
	}

	m_strData.resize(iCount);//空间调到合适大小 

	if ('-' == cSybom)
	{
		m_llValue = 0 - m_llValue;
	}
}

BigData BigData::operator+(BigData& bigData)
{
	// 8 + -2  10
	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())//相加的两个数据都未溢出
	{
		if (m_strData[0] != bigData.m_strData[0])
		{
			return BigData(m_llValue+bigData.m_llValue);//编译器支持
		}
		else//同号
		{
			// 2 + 8  10 - 6 > 2
			// -3 + -8  -10 - (-6) = -4 < -3
			if (('+' == m_strData[0] && MAX_INT64 - m_llValue >= bigData.m_llValue) ||
				('-') == m_strData[0] && MIN_INT64 - m_llValue <= bigData.m_llValue)
			{
				return BigData(m_llValue+bigData.m_llValue);
			}
		}
	}
	
	//溢出至少一个
	// 2 + 2 / -2 + -2 == -(2+2)
	// 2 + -1 
	std::string strRet;
	if (m_strData[0] == bigData.m_strData[0])
	{
		strRet = Add(m_strData, bigData.m_strData);
	}
	else//不同号
	{
		strRet = Sub(m_strData, bigData.m_strData);
	}

	return BigData(strRet.c_str());
}

BigData BigData::operator-(const BigData& bigData)
{
	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		if (m_strData[0] == bigData.m_strData[0])
		{
			return BigData(m_llValue - bigData.m_llValue);
		}
		else
		{
			// 10 + (-8) = 2 > 1// 3 - (-8); 1 - (-8) 
			// -10  -8  3    -8  2  -10 + 3 = -7 <= 
			if (('+' == m_strData[0] && MAX_INT64 + bigData.m_llValue >= m_llValue) ||
				('-' == m_strData[0] && MIN_INT64 + bigData.m_llValue <= m_llValue))
			{
				return BigData(m_llValue - bigData.m_llValue);
			}
		}
	}

	// 1、至少有一个操作数溢出
	// 2、相减的结果一定会溢出
	// "999999999" "-111111"  "-9999999" "1111111"
	std::string strRet;
	if (m_strData[0] != bigData.m_strData[0])
	{
		strRet = Add(m_strData, bigData.m_strData);
	}
	else
	{
		strRet = Sub(m_strData, bigData.m_strData);
	}
	return BigData(strRet.c_str());
}

BigData BigData::operator*(const BigData& bigData)
{
	if (0 == m_llValue || 0 == bigData.m_llValue)
	{
		return BigData(INT64(0));
	}

	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		if (m_strData[0] == bigData.m_strData[0])//同号
		{
			// 10 /2 = 5 >= 1 2 3 4 5
			// 10 /-2 = -5 <= -5 -4 -3 -2 -1 
			if (('+' == m_strData[0] && MAX_INT64 / m_llValue >= bigData.m_llValue) ||
				('-' == m_strData[0] && MAX_INT64 / m_llValue <= bigData.m_llValue))
			{
				return BigData(m_llValue*bigData.m_llValue);
			}
		}
		else//不同号
		{
			// -10 /2 = -5 <= 
			// -10/-2 = 5 >
			if (('+' == m_strData[0] && MIN_INT64 / m_llValue <= bigData.m_llValue) ||
				('-' == m_strData[0] && MIN_INT64 / m_llValue >= bigData.m_llValue))
			{
				return BigData(m_llValue*bigData.m_llValue);
			}
		}
	}

	return BigData(Mul(m_strData, bigData.m_strData).c_str());
}

//除
BigData BigData::operator /(const BigData& bigData)
{
	if (0 == bigData.m_llValue)
	{
		assert("除数不能为0！");
		return BigData(INT64(0));
	}

	if (!IsINT64Owerflow() && !bigData.IsINT64Owerflow())
	{
		return BigData(m_llValue / bigData.m_llValue);
	}

	return BigData(Div(m_strData, bigData.m_strData).c_str());
}

std::string BigData::Add(std::string left, std::string right)
{
	int iLSize = left.size();
	int iRSize = right.size();
	if (iLSize < iRSize)
	{
		std::swap(left, right);
		std::swap(iLSize, iRSize);
	}
	//88888888888888888888888
//	+                11111111


	std::string strRet;
	strRet.resize(iLSize+1);//给对象分派空间 相加的结果 最多就是左边的多一位 进位啊
	strRet[0] = left[0];
	char cStep = 0;//进位

	//left = "+9999999"  size = 9 
	// right="1"   "+10000000" 
	for (int iIdx = 1; iIdx < iLSize; ++iIdx)
	{
		char cRet = left[iLSize - iIdx] - '0' + cStep;

		if (iIdx < iRSize)
		{
			cRet += (right[iRSize - iIdx] - '0');
		}

		strRet[iLSize - iIdx + 1] = (cRet % 10 + '0');
		cStep = cRet/10;
	}
	//加上进位 之后还可能进位 
	strRet[1] = (cStep + '0');

	return strRet;
}

std::string BigData::Sub(std::string left, std::string right)
{
	// 1、左操作数 > 右操作数
	// 2、确定符号位
	int iLSize = left.size();
	int iRSize = right.size();
	char cSymbol = left[0];
	if (iLSize < iRSize || 
		(iLSize == iRSize && left < right))
	{
		std::swap(left, right);
		std::swap(iLSize, iRSize);
		if ('+' == cSymbol)
		{
			cSymbol = '-';
		}
		else
		{
			cSymbol = '+';
		}
	}


	std::string strRet;
	strRet.resize(iLSize);
	strRet[0] = cSymbol;

	// 逐位相减
	// 1、取left每一位，从后往前取
	// 2、在right没有超出  取right每一位 从后往前取
	// 3、直接相减
	// 4、 保存结果
	for (int iIdx = 1; iIdx < iLSize; iIdx++)
	{
		char cRet = left[iLSize - iIdx] - '0';
		if (iIdx < iRSize)
		{
			cRet -= (right[iRSize - iIdx] - '0');
		}

		if (cRet < 0)
		{
			left[iLSize - iIdx - 1] -= 1;
			cRet += 10;
		}

		strRet[iLSize - iIdx] = (cRet + '0');
	}

	return strRet;
}

std::string BigData::Mul(std::string left, std::string right)
{
	int iLSize = left.size();
	int iRSize = right.size();
	if (iLSize > iRSize)
	{
		std::swap(left, right);
		std::swap(iLSize, iRSize);
	}

	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}

	std::string strRet;
	//strRet.resize(iLSize + iRSize - 1);
	strRet.assign(iLSize + iRSize - 1, '0');
	strRet[0] = cSymbol;
	int iDataLen = strRet.size();
	int iOffset = 0;
		
	//短操作数 放在外循环
	for (int iIdx = 1; iIdx < iLSize; ++iIdx)
	{
		char cLeft = left[iLSize - iIdx] - '0';
		char cStep = 0;
		if (0 == cLeft)
		{
			iOffset++;
			continue;
		}

		for (int iRIdx = 1; iRIdx < iRSize; ++iRIdx)
		{
			char cRet = cLeft*(right[iRSize - iRIdx] - '0');
			cRet += cStep;
			cRet += (strRet[iDataLen - iOffset - iRIdx] - '0');
			strRet[iDataLen - iOffset - iRIdx] = cRet%10 + '0';
			cStep = cRet/10;
		}

		strRet[iDataLen - iOffset - iRSize] += cStep;
		iOffset++;
	}

	return strRet;
}


void BigData::INT64ToString()
{
	//12345
	char cSymbol = '+';
	INT64 temp = m_llValue;
	if (temp < 0)
	{
		cSymbol = '-';
		temp = 0 - temp;
	}

	m_strData.append(1, cSymbol);
	int iCount = 1;
	// 54321
	while(temp)   //追加结果吧原数弄反了
	{
		m_strData.append(1, temp%10 + '0');
		temp /= 10;
	}

	//交换过来
	char *pLeft = (char*)(m_strData.c_str()+1); 
	char *pRight = (char*)(m_strData.c_str()+m_strData.size() - 1);
	while(pLeft < pRight)
	{
		char ctemp = *pLeft;
		*pLeft++ = *pRight;
		*pRight-- = ctemp;
	}

	// 1 符号位
	// 2 m_strData = 54321
}


std::string BigData::Div(std::string left, std::string right)
{
	char cSymbol = '+';
	if (left[0] != right[0])
	{
		cSymbol = '-';
	}

	int iLSize = left.size();
	int iRSize = right.size();
	if (iLSize < iRSize ||
		iLSize == iRSize && strcmp(left.c_str() + 1, right.c_str() + 1) < 0)
	{
		return "0";
	}
	else
	{
		if ("+1" == right || "-1" == right)
		{
			left[0] = cSymbol;
			return left;
		}
	}

	std::string strRet;
	strRet.append(1, cSymbol);
	char *pLeft = (char*)(left.c_str() + 1);
	char *pRight = (char*)(right.c_str() + 1);
	int iDataLen = 1;
	iLSize -= 1;
	// "2422222222"  33
	for (int iIdx = 0; iIdx < iLSize;)
	{
		if ('0' == *pLeft)
		{
			strRet.append(1, '0');
			pLeft++;
			iIdx++;

			continue;
		}

		if (!IsLeftStrBig(pLeft, iDataLen, pRight, iRSize - 1))
		{
			strRet.append(1, '0');
			iDataLen++;
			if (iIdx + iDataLen > iLSize)
			{
				break;
			}
			continue;
		}
		else
		{
			// 循环相减
			strRet.append(1, SubLoop(pLeft, iDataLen, pRight, iRSize - 1));

			// pLeft
			while ('0' == *pLeft && iDataLen > 0)
			{
				pLeft++;
				iIdx++;
				iDataLen--;
			}

			iDataLen++;
			if (iIdx + iDataLen > iLSize)
			{
				break;
			}
		}
	}

	return strRet;
}

bool BigData::IsLeftStrBig(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(NULL != pLeft && NULL != pRight);
	if (LSize > RSize ||
		LSize == RSize && strncmp(pLeft, pRight, LSize) >= 0)
	{
		return true;
	}

	return false;
}

char BigData::SubLoop(char *pLeft, size_t LSize, char *pRight, size_t RSize)
{
	assert(NULL != pLeft && NULL != pRight);

	char cRet = '0';
	while (true)
	{
		if (!IsLeftStrBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}

		// 做-=
		int iLDataLen = LSize - 1;
		int iRDataLen = RSize - 1;
		while (iRDataLen >= 0 && iLDataLen >= 0)
		{
			if (pLeft[iLDataLen] < pRight[iRDataLen])
			{
				pLeft[iLDataLen - 1] -= 1;
				pLeft[iLDataLen] += 10;
			}

			pLeft[iLDataLen] = pLeft[iLDataLen] - pRight[iRDataLen] + '0';
			iLDataLen--;
			iRDataLen--;
		}

		// "990000000000000000000000000099"
		while ('0' == *pLeft && LSize > 0)
		{
			pLeft++;
			LSize--;
		}

		cRet++;
	}

	return cRet;
}


bool BigData::IsINT64Owerflow() const
{
	std::string strTemp;
	if ('+' == m_strData[0])
	{
		strTemp = "+9223372036854775807";//64位 正的最大
	}
	else
	{
		strTemp = "-9223372036854775808";//64位 小的最小
	}

	if (m_strData.size() > strTemp.size())
	{
		return true;
	}
	else if (m_strData.size() == strTemp.size() && m_strData > strTemp)
	{
		return true;
	}

	return false;
}

std::ostream& operator<<(std::ostream& _cout, const BigData& bigData)
{
	if (!bigData.IsINT64Owerflow()) // 没有溢出
	{
		_cout<<bigData.m_llValue;
	}
	else
	{
		char* pData = (char*)bigData.m_strData.c_str();
		if (pData[0] == '+')
		{
			pData++;
		}
		_cout<<pData;
	}
	return _cout;
}


void mymath(char *arg)
{
    char *data[3] = {0};
    int i = 2;

    char *end = arg + strlen(arg) - 1;
    while(arg < end)
    {
	if(*end == '=')
	{
	    data[i--] = end+1;
	}

	if(*end == '&')
	{
	    *end = '\0';
	}
	end--;
    }

    printf("data1 =  %s\n",data[0]);
    printf("data2 =  %s\n",data[1]);

   // BigData s1(data[0]);
   // BigData s2(data[1]);

    if(strcmp(data[2],"%2B") == 0)
    {
	printf("<html>\n");
	printf("<h1>");
	printf("%s + %s = %d",data[0],data[1],atoi(data[0])+atoi(data[1]));
	printf("</h1>\n");
	printf("</html>\n");
    }
}



void Test1()
{
    char method[1024];
    char arg[1024];
    char content_len[1024];
    int len = -1;

    if(getenv("REQUEST_METHOD"))
    {
	strcpy(method,getenv("REQUEST_METHOD"));
    }

    if(strcasecmp(method,"GET") == 0)
    {
	if(getenv("QUERY_STRING"))
	{
	    strcpy(arg,getenv("QUERY_STRING"));
	}
    }
    else
    {
	if(getenv("CONTENT_LENGTH"))
	{
	    strcpy(content_len,getenv("CONTENT_LENGTH"));
	    len = atoi(content_len);
	}

	int i = 0;
	for(; i < len; i++)
	{
	    read(0,&arg[i],1);
	}
	arg[i] = '\0';
    }

    mymath(arg);

}

int main()
{
	Test1();
	return 0;
}
