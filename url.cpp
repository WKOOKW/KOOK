#include "url.h"

namespace kook{

	URL::URL()
	{
		memset(buf , 0 , 5);
		url_error = URL_INPUT_VALUE_NULL;
		url_data = "";
	}

	URL::URL(const char *url , size_t url_size)
	{
		memset(buf , 0 , 5);
		url_error = URL_INPUT_VALUE_NULL;
		url_data = "";
		if(NULL == url)
			return;

		url_data.assign(url , url_size);
		url_error = 0;
	}

	bool URL::url_input(const char *url , size_t url_size)
	{
		if(NULL == url){
			url_error = URL_INPUT_VALUE_NULL;
			url_data = "";
			return false;
		}

		url_data.assign(url , url_size);
		url_error = 0;
		return true;
	}

	int URL::url_get_error()
	{
		return url_error;
	}

	string URL::url_get_encode(URL_SAFETY safety)
	{
		return url_encode(safety);
	}

	string URL::url_get_decode(bool y /* = false */)
	{
		return url_decode(y);
	}

	char URL::generate_hex(int m)
	{
		if(m >= 0 && m < 10)
			return (char)(m + 0x30);
		if(m >= 10 && m < 16)
			return (char)(m + 0x37);
		return 0;
	}

	bool URL::is_hex_value(const char h)
	{
		if((h >= 0x30 && h <= 0x39) || (h >= 0x61 && h <= 0x66) || (h >= 0x41 && h <= 0x46))
			return true;   //如果字符是在hex值的范围内就返回真
		return false;
	}

	const char * URL::char_to_url_hex(const char url_char)
	{
		buf[0] = '%';
		buf[1] = generate_hex(((url_char & 0x000000f0)>>4));
		buf[2] = generate_hex((url_char & 0x0000000f));
		buf[3] = '\0';
		return buf;
	}

	const char URL::url_hex_to_char(const char *url_hex)
	{
		int m = 0;
		m = strtol(url_hex , NULL , 16);
		return (char)m;
	}

	bool URL::url_char_inspect(const char url_char , URL_SAFETY safety)
	{
		if(URL_SAFETY_CHSR5 == safety)
			return true;       //需要编码返回true

		if((url_char >= 0x30 && url_char <= 0x39) ||
		   (url_char >= 0x41 && url_char <= 0x5A) ||
		   (url_char >= 0x61 && url_char <= 0x7A)){
			   return false;  //不需要编码返回false
		}

		switch(safety)
		{
		case URL_SAFETY_CHSR1:
			//!*();:@&=+$,/?[]~.-_
			if(url_char == 0x21 || url_char == 0x24 || url_char == 0x26 ||
				(url_char >= 0x28 && url_char <= 0x2F) || url_char == 0x3A ||
				url_char == 0x3B || url_char == 0x3D || url_char == 0x3F ||
				url_char == 0x40 || url_char == 0x5B || url_char == 0x5D ||
				url_char == 0x5F || url_char == 0x7E){
					return false;
			}
			return true;
		case URL_SAFETY_CHSR2:
			//!()*-._~
			if(url_char == 0x21 || url_char == 0x28 || url_char == 0x29 ||
			   url_char == 0x2A || url_char == 0x2D || url_char == 0x2E ||
			   url_char == 0x5F || url_char == 0x7E){
				   return false;
			}
			return true;
		case URL_SAFETY_CHSR3:
			//!#$&'()*+,-./:;=?@[]_~
			if(url_char == 0x21 || url_char == 0x23 || url_char == 0x24 ||
			  (url_char >= 0x26 && url_char <= 0x2F) || url_char == 0x3A ||
			   url_char == 0x3B || url_char == 0x3D || url_char == 0x3F ||
			   url_char == 0x40 || url_char == 0x5B || url_char == 0x5D ||
			   url_char == 0x5F || url_char == 0x7E){
				   return false;
			}
			return true;
		case URL_SAFETY_CHSR4:
			return true;
		}
		return true;
	}

	string URL::url_encode(URL_SAFETY safety)
	{
		if(URL_INPUT_VALUE_NULL == url_error)
			return string();
		url_error = 0;

		string ret("");
		int length = url_data.size() , index = 0;
		char b = 0;
		while(index < length)
		{
			b = url_data.at(index);
			if(url_char_inspect(b , safety)){
				//如果返回true就说明需要对字符进行编码
				ret += char_to_url_hex(b);
			}else{
				//如果返回false就说明不需要编码
				ret += b;
			}
			++index;
		}
		return ret;
	}

	string URL::url_decode(bool y)
	{
		if(URL_INPUT_VALUE_NULL == url_error)
			return string();
		url_error = 0;

		string ret("");
		int length = url_data.size() , index = 0;
		char b = 0;
		while(index < length)
		{
			b = url_data.at(index);
			if('%' == b){//读取到%号就表示后面两位是需要解码的数据
				if((index + 2) >= length){
					url_error = URL_DECODE_HEX_VALUE;
					return string();
				}
				if(!(is_hex_value((buf[0] = url_data.at(++index))) && is_hex_value((buf[1] = url_data.at(++index))))){
					url_error = URL_DECODE_HEX_VALUE;
					return string();
				}
				buf[2] = 0;
				ret += url_hex_to_char(buf);
				++index;
				continue;
			}else if('+' == b){
				if(y){//如果需要将+号转换为空格
					ret += (char)0x20;
					++index;
					continue;
				}
			}
			ret += b;
			++index;
		}
		return ret;
	}
}
