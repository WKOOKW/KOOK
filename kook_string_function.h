#ifndef KOOK_STRING_FUNCTION_H_H
#define KOOK_STRING_FUNCTION_H_H

/*
字符串功能函数
实现：不懂
*/

namespace kook {
	class kook_string_function {
	public:
		// 按指定字符来迭代遍历字符串
		// str需要遍布的字符串
		// s是迭代的分隔符
		// out_buf迭代的字符串将保存到这个缓冲区
		// buf_size指定缓冲区大小，如果缓冲区不够大，数据将丢失
		// in用于指定是否在outBuf中将分隔符带上，true表示在结果中带上分隔符
		// 成功返回余下的字符串，返回或结束返回NULL
		static const char *iteration_str(const char *str, char s, char out_buf[], unsigned int buf_size, bool in = false);

		// 查找指定字符在字符串中第一次出现的位置
		// str是需要被查找的字符串
		// s是需要查找的字符
		// 查找失败返回-1，成功返回下标
		// 注意：最好不要查找长度超出int表示范围的字符串，否则有可能int会溢出
		static int find_str(const char *str, char s);

		// 从str中截取字符串，截取的范围是从字符串开始到cut符结束，但不包括cut符号
		// 如果所有字符都查找完毕都没有发现cut字符，将返回false，但outBuf中会存放已经查找过的字符串
		// 注意：如果outBuf长度不够，将会丢失一部分数据
		// str需要处理的字符串
		// cut指定截取结束字符
		// out_buf截取字符串保存的缓冲区
		// buf_size缓冲区大小
		static bool interception_str(const char *str, const char cut, char out_buf[], unsigned int buf_size);

		// 判断字符是否是一个英文字母(a-z)，不区分大小写
		// 返回true：是一个英文字母
		static bool is_alpha(char ch);
		// 判断一个字符是否是大写英文字母(A-Z)
		// 返回true：是一个大写英文字母
		static bool is_upper(char ch);
		// 判断一个字符是否是小写英文字母(a-z)
		// 返回true：是一个小写英文字母
		static bool is_lower(char ch);
		// 判断一个字符是否是数字(0-9)
		// 返回true：是一个数字
		static bool is_number(char ch);
		// 判断一个字符是否是一个16进制数字符，即(0-f)
		// 返回true：是一个16进制数
		static bool is_hex_number(char ch);
		// 判断一个字符是否是字母或数字
		// 返回true：是一个字母或数字
		static bool is_alnum(char ch);
		// 判断一个字符是否是可打印字符
		// 返回true：是一个可打印的字符
		static bool is_print(char ch);
	};
}

#endif // KOOK_STRING_FUNCTION_H_H