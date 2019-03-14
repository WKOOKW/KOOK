#include "kook_string_function.h"

namespace kook {
	const char *kook_string_function::iteration_str(const char *str, char s, char out_buf[], unsigned int buf_size, bool in) {
		if (0 == str || 0 == buf_size || 0 == (*str)) {
			return 0;
		}
		const char *str_pt = str;
		unsigned int index = 0;
		while (0 != (*str_pt) && s != (*str_pt))
		{
			if (index < buf_size - 1) {
				out_buf[index++] = *str_pt;
			}
			str_pt++;
		}
		if (in && s == (*str_pt) && index < (buf_size - 2)) {
			out_buf[index++] = s;
		}
		out_buf[index] = 0;
		if (0 == (*str_pt)) {
			return str_pt;
		}
		str_pt++;
		return str_pt;
	}

	int kook_string_function::find_str(const char *str, char s) {
		if (0 == str) {
			return -1;
		}
		const char *pt = str;
		int str_pos = 0;
		while (0 != (*pt))
		{
			if (s == (*pt)) {
				return str_pos;
			}
			pt++;
			str_pos++;
		}
		return -1;
	}

	bool kook_string_function::interception_str(const char *str, const char cut, char out_buf[], unsigned int buf_size) {
		if (0 == str) return false;
		const char *pt = str;
		unsigned int index = 0;
		while ((*pt) != cut && (*pt) != '\0') {
			if (index < buf_size -1) {
				out_buf[index++] = *pt;
			}
			pt++;
		}
		out_buf[index] = '\0';
		if ((*pt) == cut) {
			return true;
		}
		return false;
	}

	bool kook_string_function::is_alpha(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0041 && tempch <= 0x005A) return true;
		if (tempch >= 0x0061 && tempch <= 0x007A) return true;
		return false;
	}

	bool kook_string_function::is_upper(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0041 && tempch <= 0x005A) return true;
		return false;
	}

	bool kook_string_function::is_lower(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0061 && tempch <= 0x007A) return true;
		return false;
	}

	bool kook_string_function::is_number(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0030 && tempch <= 0x0039) return true;
		return false;
	}

	bool kook_string_function::is_hex_number(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0030 && tempch <= 0x0039) return true;
		if (tempch >= 0x0041 && tempch <= 0x0046) return true;
		if (tempch >= 0x0061 && tempch <= 0x0066) return true;
		return false;
	}

	bool kook_string_function::is_alnum(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0041 && tempch <= 0x005A) return true;
		if (tempch >= 0x0061 && tempch <= 0x007A) return true;
		if (tempch >= 0x0030 && tempch <= 0x0039) return true;
		return false;
	}

	bool kook_string_function::is_print(char ch) {
		short tempch = (short)ch;
		if (tempch >= 0x0020 && tempch <= 0x007E) return true;
		return false;
	}
}