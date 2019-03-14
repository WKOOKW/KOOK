#include "hex.h"

namespace kook
{
	HEX::HEX()
	{
		hex_error = HEX_INPUT_VALUE_NULL;
		hex_text = "";
		hex_mode = 0x00000007;
	}

	HEX::HEX(const char *data , size_t n)
	{
		hex_error = 0;
		hex_mode = 0x00000007;
		if(NULL == data){
			hex_text = "";
			hex_error = HEX_INPUT_VALUE_NULL;
		}else{
			hex_text.assign(data , n);  //在给string传值时，如果buf是data的就需要使用这个函数来指定数据的长度
		}
	}

	bool HEX::hex_input(const char *data , size_t n)
	{
		if(NULL == data){
			hex_text = "";
			hex_error = HEX_INPUT_VALUE_NULL;
			return false;
		}
		hex_text.assign(data , n);   //在给string传值时，如果buf是data的就需要使用这个函数来指定数据的长度
		hex_error = 0;
		return true;
	}

	bool HEX::set_hex_mode(int HEXMODE , bool value)
	{
		if(value)
			hex_mode = hex_mode | HEXMODE;       //设置
		else
			hex_mode = hex_mode & (~HEXMODE);    //取消设置
		return true;
	}

	int HEX::get_hex_error()
	{
		return hex_error;
	}

	int HEX::get_hex_mode()
	{
		return hex_mode;
	}

	string HEX::get_to_hex()
	{
		return text_to_hex();
	}

	string HEX::get_to_text()
	{
		return hex_to_text();
	}

	int HEX::get_hex_limits(char h)
	{
		if((h >= 0x30 && h <= 0x39) || (h >= 0x61 && h <= 0x66) || (h >= 0x41 && h <= 0x46))
			return 0;

		if(0x20 == h)
			return 1;

		return 2;
	}

	char HEX::generate_hex(int m)
	{
		if(m >= 0 && m < 10)
			return (char)(m + 0x30);
		if(m >= 10 && m < 16)
			return (char)(m + 0x37);
		return ' ';
	}

	char *HEX::get_hex_result(char r[3] , bool y)
	{
		int m = 0;
		if(y){    //hex_to_text
			m = strtol(r , NULL , 16);
			hex_buf[0] = (char)m;
			hex_buf[1] = '\0';
			return hex_buf;
		}
		//text_to_hex
		m = r[0];
		hex_buf[0] = generate_hex(((m & 0x000000f0)>>4));
		hex_buf[1] = generate_hex((m & 0x0000000f));
		hex_buf[2] = '\0';
		return hex_buf;
	}

	string HEX::hex_to_text()
	{
		if((int)HEX_INPUT_VALUE_NULL == hex_error)
			return string();

		size_t len = hex_text.size();
		size_t index = 0 , m = 0;
		char buf[3];
		bool make = false;   //false表示第一位，true表示第二位
		string ret("");      //用于保存转换后的结果

		while(index < len)
		{
			m = get_hex_limits(hex_text.at(index));

			if(((index + 1) == len) && !make && m == 0){  //当读取到最后一位时
				if(HEX_TO_TEXT_AUTO_ADD_O == (hex_mode & HEX_TO_TEXT_AUTO_ADD_O)){  //如果设置了自动在前面补0
					buf[0] = '0';
					buf[1] = hex_text.at(index);
					buf[2] = '\0';
					ret.append(get_hex_result(buf , true) , 1);
					return ret;
				}else{   //如果没有设置就返回错误
					hex_error = HEX_TEXT_VALUE_MISMATCHING;
					return string();
				}
			}

			if(0 == m){    //读取到正常的Hex值
				if(!make){
					buf[0] = hex_text.at(index);
					make = true;
					++index;
					continue;
				}else{
					buf[1] = hex_text.at(index);
					buf[2] = '\0';
					ret.append(get_hex_result(buf , true) , 1);
					make = false;
					++index;
					continue;
				}
			}else if(1 == m){      //读取到空格时
				if(HEX_TO_TEXT_ELIDE_BLANK != (hex_mode & HEX_TO_TEXT_ELIDE_BLANK)){  //如果设置了不忽略空格就返回错误
					hex_error = HEX_TEXT_VALUE_BLANK;
					return string();
				}
				if(!make){   //如果是起始位就忽略空格
					++index;
					continue;
				}else if(make && (HEX_TO_TEXT_AUTO_ADD_O == (hex_mode & HEX_TO_TEXT_AUTO_ADD_O))){   //如果是第二位就判断是否要在前面补0
					buf[1] = buf[0];
					buf[0] = '0';
					buf[2] = '\0';
					ret.append(get_hex_result(buf , true) , 1);
					make = false;
					++index;
					continue;
				}else if(make && (HEX_TO_TEXT_AUTO_ADD_O != (hex_mode & HEX_TO_TEXT_AUTO_ADD_O))){   //如果是第二位并不需要补0
					hex_error = HEX_TEXT_VALUE_MISMATCHING;
					return string();
				}
			}else if(2 == m){      //如果读取到其它字符就直接返回错误
				hex_error = HEX_TEXT_VALUE_OVERPROOF;
				return string();
			}
		}
		hex_error = 0;
		return ret; 
	}

	string HEX::text_to_hex()
	{
		if((int)HEX_INPUT_VALUE_NULL == hex_error)
			return string();

		size_t len = hex_text.size();
		size_t index = 0;
		string ret("");
		char buf[3];
		while(index < len)
		{
			buf[0] = hex_text.at(index);
			ret += get_hex_result(buf , false);
			if(TEXT_TO_HEX_ADD_BLANK == (hex_mode & TEXT_TO_HEX_ADD_BLANK)){
				ret += ' ';
			}
			++index;
		}
		hex_error = 0;
		return ret;
	}
}
