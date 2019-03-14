#ifndef KOOK_SHARE_RESOURCE_H_H
#define KOOK_SHARE_RESOURCE_H_H

/*
其它功能
实现：不懂
*/

#ifdef WIN32
// 忽略localtime等函数的错误
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#else
#include <unistd.h>
#include <stdio.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <string>
using std::string;

// 一个通用的字符串格式化宏
#ifdef _WIN32
#define STRFORMAT sprintf_s
#else
#define STRFORMAT snprintf
#endif

namespace kook {

	class share_resource {
	public:
		// 睡眠指定时间，单位为毫秒
		static bool sleep(unsigned int time = 1000);
		// 获取主机cpu核心数量
		static unsigned int get_cpu_core_number();
	};

	//日期
	class kook_data {
	public:
		int year;     //年
		int month;    //月
		int day;      //日
		kook_data() : year(-1), month(-1), day(-1) {}
		bool empty() {
			if (year == -1 || month == -1 || day == -1) return true;
			return false;
		}
	};
	//时间
	class kook_time {
	public:
		int hour;     //时
		int minute;   //分
		int second;   //秒
		kook_time() :hour(-1), minute(-1), second(-1) {}
		bool empty() {
			if (hour == -1 || minute == -1 || second == -1) return true;
			return false;
		}
	};
	//时期时间
	class kook_data_time {
	public:
		kook_data date;
		kook_time time;
		bool empty() {
			if (date.empty() || time.empty()) {
				return true;
			}
			return false;
		}
	};


	//日期时间时间戳转换对象，转换后的日期时间是中国本地时区
	//这里没有处理其它时区
	class simple_time {
	public:
		//一天的秒数
		static const int DAYSECOND = 24 * 60 * 60;
		//一小时的秒数
		static const int HOURSECOND = 60 * 60;
		//一分针的秒数
		static const int MINUTESECOND = 60;
		//一个周年的总天数
		static const int FOURYEARS = 365 + 365 + 366 + 365;
		//中国的时区比uct多8个时间
		static const int CHTZ = 8 * 60 * 60;
		//日期的起始年份
		static const int STARTYEAR = 1970;
		//每月天数字典，这只是平年的天数
		static const char MONTHDICT[12];

		//获取当前系统时间截
		static time_t get_time_int();
		//使用系统函数来计算时间
		static struct tm get_time_tm();
		static string get_date_time_str(struct tm &t);
		static string get_date_time_str(time_t t);
		static string get_date_str(struct tm &t);
		static string get_time_str(struct tm &t);
		//获取当前系统时间的字符串
		static bool get_date_time(time_t time_int, kook_data_time &DT);
		//获取当前系统时间的日期部分
		static bool get_date(time_t time_int, kook_data &D);
		//获取当前系统时间的时间部分
		static bool get_time(time_t time_int, kook_time &T);
	};


	// 一个简单封装的随机数生成对象
	//
	class simple_rand_int {
	public:
		simple_rand_int() : min_number(0), max_number(RAND_MAX), rand_count(0) {
			// 初始化随机数种子
			srand((unsigned int)time(0));
		}
		simple_rand_int(const simple_rand_int &r) {
			min_number = r.min_number;
			max_number = r.max_number;
			// 每拷贝一次就初始化一次随机种子
			srand((unsigned int)time(0));
			rand_count = 0;
		}
		const simple_rand_int& operator=(const simple_rand_int &r) {
			min_number = r.min_number;
			max_number = r.max_number;
			// 每拷贝一次就初始化一次随机种子
			srand((unsigned int)time(0));
			rand_count = 0;
			return r;
		}
		// 重置随机种子
		void reset_srand() {
			srand((unsigned int)time(0));
			rand_count = 0;
		}
		// 设置随机数生成范围
		bool set_range(int min_n, int max_n);
		// 生成指定范围随机数
		int randint();
	protected:
		int min_number;
		int max_number;
		int rand_count;
	};

}

#endif
