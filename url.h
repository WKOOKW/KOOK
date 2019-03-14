#ifndef KOOK_URL_H_H
#define KOOK_URL_H_H

/*
Url编码
实现：不懂
*/

#include <string>
using std::string;
#include <string.h>
#include <stdlib.h>

namespace kook{

	enum URL_ERROR
	{
		URL_INPUT_VALUE_NULL = 0x80000001,           //输入的data为null
		URL_DECODE_HEX_VALUE = 0x80000002,           //在进行url解码时发现hex值不成对或超出hex值范围
	};

	enum URL_SAFETY
	{//URL_SAFETY表示可出现在url中的安全字符(即不用编码的字符)，在RFC3986与java中'是不需要编码，但浏览器与网上其它url编码都对'进行了编码，这里就也进行编码
		URL_SAFETY_CHSR1,        //可出现的安全字符(0-9|a-z|A-Z|!*();:@&=+$,/?[]~.-_),通过查看RFC3986与抓取浏览器发送的url来看,url中可以出现这些字符(与java中encodeURI函数类似)
		URL_SAFETY_CHSR2,        //可出现的安全字符(0-9|a-z|A-Z|!()*-._~)，与java中encodeURIComponent类似
		URL_SAFETY_CHSR3,        //可出现的安全字符(0-9|a-z|A-Z|-_.~!*'();:@&=+$,/?#[])，完全按照RFC3986规定
		//RFC3986文档规定，Url中只允许包含英文字母(a-zA-Z),数字(0-9),4个特殊字符-_.~以及所有保留字符!*'();:@&=+$,/?#[]
		URL_SAFETY_CHSR4,        //可出现的安全字符(0-9|a-z|A-Z),提供特殊要求的url编码
		URL_SAFETY_CHSR5,        //对所有字符进行(%0x**)编码,提供特殊要求的url编码
	};

	class URL
	{
	public:
		URL();
		URL(const char *url , size_t url_size);
		//接收url
		bool url_input(const char *url , size_t url_size);
		//获取url编码后的结果
		string url_get_encode(URL_SAFETY safety = URL_SAFETY_CHSR1);
		//获取url解码后的结果(是否需要处理+号！！！(y=trye表示处理(即将+号转换为空格)，y=false表示不做处理))
		string url_get_decode(bool y = false);
		//获取错误值
		int url_get_error();
	protected:
		bool is_hex_value(const char h);
		//
		char generate_hex(int m);
		//将一个字符转换为url形式的hex值
		const char *char_to_url_hex(const char url_char);
		//将url形式的hex值转换为字符
		const char url_hex_to_char(const char *url_hex);
		//检查一个字符是否需要编码，需要编码返回trur，不需要编码返回false
		bool url_char_inspect(const char url_char , URL_SAFETY safety);
		//url编码
		string url_encode(URL_SAFETY safety);
		//url解码
		string url_decode(bool y);

		char buf[5];
		int url_error;
		string url_data;
	};
}

#endif