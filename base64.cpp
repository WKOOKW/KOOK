#include "base64.h"

namespace kook
{
	base64::base64()
	{
		base64_data = "";
		memset(base64_buf , 0 , 5);
		base64_error = BASE64_INPUT_VALUE_NULL;
	}

	base64::base64(const char *data , size_t l)
	{
		base64_data = "";
		memset(base64_buf , 0 , 5);
		base64_error = BASE64_INPUT_VALUE_NULL;
		if(NULL == data)
			return;

		base64_data.assign(data , l);
		base64_error = 0;
	}

	bool base64::base64_input(const char *data , size_t l)
	{
		if(NULL == data)
		{
			base64_data = "";
			base64_error = BASE64_INPUT_VALUE_NULL;
			return false;
		}

		base64_data.assign(data , l);
		base64_error = 0;
		return true;
	}

	int base64::base64_get_error()
	{
		return base64_error;
	}

	char base64::base64_get_char(const char b)
	{
		if(b >= 0x00 && b <= 0x19)
			return (b + 0x41);
		if(b >= 0x1A && b <= 0x33)
			return (b - 0x1A + 0x61);
		if(b >= 0x34 && b <= 0x3D)
			return (b - 0x34 + 0x30);
		if(b == 0x3E)
			return '+';
		if(b == 0x3F)
			return '/';

		return 0;
	}

	char base64::base64_get_value(const char b)
	{
		if(b >= 0x41 && b <= 0x5A)
			return b - 0x41;
		if(b >= 0x61 && b <= 0x7A)
			return b - 0x61 + 0x1A;
		if(b >= 0x30 && b <= 0x39)
			return b - 0x30 + 0x34;
		if(b == 0x2B)
			return 0x3E;
		if(b == 0x2F)
			return 0x3F;

		return 0;
	}

	bool base64::base64_checkout(char c)
	{
		if((c >= 0x41 && c <= 0x5A) ||
		   (c >= 0x61 && c <= 0x7A) ||
		   (c >= 0x30 && c <= 0x39) ||
		   c == 0x2B || c == 0x2F ){
			   return true;
		}
		return false;
	}

	const char *base64::base64_get_array(const char b[5] , bool make , int d)
	{
		int i = 0 , j = 0;
		char new_b = 0 , buf = 0;
		if(make){//编码
			j = 3-d;
			while(i < j)
			{
				new_b = (b[i] >> (2*(i+1)));      //取出i位
				new_b = new_b & (0x3F >> 2*i);    //将最高位清0，防止最高位为1时的算术右移
				base64_buf[i] = base64_get_char(new_b | buf); //加入上一位移出的数据
				buf = b[i] & (0x3F >> (6 - 2*(i+1)));         //取出需要放到下一位的数据
				buf = buf << (6 - 2*(i+1));                   //调整位置,以方便直接与下一位做|运算
				++i;
			}
			base64_buf[i] = base64_get_char(0x00 | buf);      //加入上一位移出的数据
			j = 0;
			while(j < d){//如果数据不是3的倍数就用=号补充
				base64_buf[++i] = '=';
				++j;
			}
			base64_buf[++i] = 0;
		}else{//解码
			j = d;
			i = d - 2;
			while(j > 0){
				if(j == d){//最后一位都是可以丢弃的
					new_b = (0x3f<<(6-(j+i)))&b[j-1];
					new_b = new_b>>(6-(j+i));
					base64_buf[j-1] = 0;
					--j;
					--i;
					continue;
				}
				//取出下几位
				buf = (0x3f<<(6-(j+i)))&b[j-1];
				buf = buf>>(6-(j+i));
				base64_buf[j-1] = (b[j-1]<<(j+i+2))|new_b;
				new_b = buf;
				--j;
				--i;
			}
		}
		return base64_buf;
	}

	string base64::base64_get_encode()
	{
		if((int)BASE64_INPUT_VALUE_NULL == base64_error)
			return string();
		base64_error = 0;

		char buf[5];
		string ret("");
		size_t length = base64_data.size();
		size_t index = 0 , i = 0;
		while(index < length)
		{//编码
			buf[i++] = base64_data.at(index);
			if((index + 1) % 3 == 0){
				ret += base64_get_array(buf);
				i = 0;
			}
			++index;
		}
		if(length % 3 != 0){//如果不是3是倍数就补=
			ret += base64_get_array(buf , true , 3 - (length % 3));
		}
		return ret;
	}

	string base64::base64_get_decode()
	{
		if((int)BASE64_INPUT_VALUE_NULL == base64_error)
			return string();
		base64_error = 0;

		char buf[5];
		string ret("");
		size_t index = 0 , i = 0;
		size_t length = base64_data.size();
		while(index < length){
			if(!base64_checkout(base64_data.at(index))){//当解码数据中遇到无法解码的数据时就停止
				if(i != 0){
					ret.append(base64_get_array(buf , false , i) , i-1);
				}
				if(base64_data.at(index) != 0x3D)
					base64_error = BASE64_DECODE_VALUE_ILLEGALITY;

				return ret;
			}
			buf[i++] = base64_get_value(base64_data.at(index));
			if((index + 1) % 4 == 0 || index + 1 == length){//当取完一组数据或数据结束时
				ret.append(base64_get_array(buf , false , i) , i-1);
				if(i < 2)
					base64_error = BASE64_DECODE_FINISH_UNUS;

				i = 0;
			}
			++index;
		}
		return ret;
	}
}

/*
#include "base64.h"
#include <iostream>
int main()
{
	kook::base64 ba;
	string rrr , ccc;
	char a[10] = {0x3D,0x41,0x00,0x00,0x3D,0x52,0x00,0x00,0x44,0x8E};
	ba.base64_input(a , 10);
	rrr = ba.base64_get_encode();
	std::cout<<rrr<<std::endl;

	//rrr = base64_encode((unsigned char const*)a , 10);
	//std::cout<<rrr<<std::endl;
	//rrr.insert(14 , "-");
	ba.base64_input(rrr.c_str() , rrr.size());
	ccc = ba.base64_get_decode();
	ccc;
	return 0;
}*/