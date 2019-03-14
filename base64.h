#ifndef KOOK_BASE64_H_H
#define KOOK_BASE64_H_H

/*
base64编码
实现：不懂
*/

#include <string>
using std::string;
#include<string.h>

namespace kook
{
	enum BASE64_ERROE
	{
		BASE64_INPUT_VALUE_NULL = 0x80000001,           //输入的data为null
		BASE64_DECODE_VALUE_ILLEGALITY = 0x80000002,    //需要解码的数据中存在非法字符
		BASE64_DECODE_FINISH_UNUS = 0x80000003,         //需要解码的数据字节数不符合base64的编码规则
	};

	class base64
	{
	public:
		base64();
		// 拷贝构造函数
		base64(base64 &b64) {
			base64_data = b64.base64_data;
			base64_error = b64.base64_error;
			memcpy(base64_buf, b64.base64_buf, 5);
		}
		// 赋值函数
		const base64& operator=(const base64 &b64) {
			base64_data = b64.base64_data;
			base64_error = b64.base64_error;
			memcpy(base64_buf, b64.base64_buf, 5);
			return b64;
		}
		//data为要转换的数据buf,l为buf长度(之所以要指定长度，是因为在编码图片或其它数据时会有大量的0x00，如果不指定string读取到0x00时结束)
		base64(const char *data , size_t l);
		bool base64_input(const char *data , size_t l);
		//调用base64_get_encode时，对象默认输入的数据是编码前的数据，调用base64_get_decode时，对你默认输入的数据是编码后的数据
		//获取将数据编码后的结果
		string base64_get_encode();
		//获取解码后的数据
		string base64_get_decode();
		//获取错误值
		int base64_get_error();
		// 清空缓存
		void clean() {
			base64_data.clear();
			memset(base64_buf, 0, 5);
			base64_error = BASE64_INPUT_VALUE_NULL;
		}
	protected:
		//将一个字节数转换为base64对应的编码
		char base64_get_char(const char b);
		//将base64编码字符还原
		char base64_get_value(const char b);
		//将一组位移好的数据转换为base64对应的编码，make为true表示编码，为false表示解码,d在进行编码时用于指定需要补位数(即加多少个=)
		const char *base64_get_array(const char b[5] , bool make = true , int d = 0);
		//检查base64编码是否在有效范围内
		bool base64_checkout(char c);

		string base64_data;     //保存用户输入数据
		char base64_buf[5];     //用于组转换时的buf
		int base64_error;       //保存错误值
	};
}

#endif