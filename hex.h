#ifndef KOOK_HEX_H_H
#define KOOK_HEX_H_H

/*
hex编码
实现：不懂
*/

#include <string>
using std::string;
#include <string.h>

#ifndef WIN32
#include <stdlib.h>
#endif

namespace kook
{
	enum HEXMODE
	{
		HEX_TO_TEXT_ELIDE_BLANK = 0x00000001,        //将hex转换为text时忽略空格(默认忽略空格)
		HEX_TO_TEXT_AUTO_ADD_O  = 0x00000002,        //将hex转换为text时，如果hex值不配对将自动在前面补充0(默认补充0)
		TEXT_TO_HEX_ADD_BLANK   = 0x00000004,        //将text转换为hex时，在hex值之间加入空格(默认加入)
	};

	enum HEXERROR
	{
		HEX_INPUT_VALUE_NULL = 0x80000001,           //输入的hex值为null
		HEX_TEXT_VALUE_OVERPROOF = 0x80000002,       //从hex转换到text时发现了非法的hex值
		HEX_TEXT_VALUE_MISMATCHING = 0x80000003,     //从hex转换到text时发现hex值不成对，如果要自动在前面补0可以进行设置
		HEX_TEXT_VALUE_BLANK = 0x80000003,           //从hex转换到text时发现hex值中含有空格，如果需要忽略空格可以进行设置
	};

	class HEX
	{
	public:
		HEX();
		// 拷贝构造函数
		HEX(const HEX &hex) {
			hex_error = hex.hex_error;
			hex_text = hex.hex_text;
			hex_mode = hex.hex_mode;
			memcpy(hex_buf, hex.hex_buf, 3);
		}
		// 赋值函数
		const HEX& operator=(const HEX &hex) {
			hex_error = hex.hex_error;
			hex_text = hex.hex_text;
			hex_mode = hex.hex_mode;
			memcpy(hex_buf, hex.hex_buf, 3);
			return hex;
		}
		//
		HEX(const char *text , size_t n);
		//输入
		bool hex_input(const char *text , size_t n);
		//设置hex的工作模式,当value为true时表示设置，value为false时表示取消设置
		//要一次设置多个值时，可以作用|位运算，如：HEX_TO_TEXT_ELIDE_BLANK|HEX_TO_TEXT_AUTO_ADD_O这样就当时设置了两个值
		bool set_hex_mode(int HEXMODE , bool value);
		//获取错误值
		int get_hex_error();
		//获取当前的mode设置值
		int get_hex_mode();
		//获取处理结果，在调用获取函数时需要注意：如调用get_to_hex时，函数会默认输入的值是text，调用get_to_text时，函数会默认输入的值是hex值
		string get_to_hex();
		string get_to_text();
		// 清空缓存
		void clean() {
			hex_error = HEX_INPUT_VALUE_NULL;
			hex_text.clear();
		}
	protected:
		//将输入的值从hex转换为text
		string hex_to_text();
		//将输入的值从text转换为hex
		string text_to_hex();
		//检查hex值的范围,0表示hex值正常，1表示空格，2表示超出hex范围
		int get_hex_limits(char h);
		//将一个值转换为hex表示
		char generate_hex(int m);
		//转换单个hex值,y为true时表示hex_to_text，y为false时表示text_to_hex
		char *get_hex_result(char r[3] , bool y);

		//data
		int hex_error;    //保存错误值
		string hex_text;  //保存用户输入的数据
		int hex_mode;     //保存用户设置(以位的形式保存，每一位代表一种模式)
		char hex_buf[3];  //用于hex转换的buf
	};
}

#endif