#include "share_resource.h"

namespace kook {

	bool share_resource::sleep(unsigned int time /* = 1000 */) {
#ifdef _WIN32
		Sleep(time);
#else
		time *= 1000;
		usleep(time);
#endif
		return true;
	}

	unsigned int share_resource::get_cpu_core_number() {
#ifdef _WIN32
		SYSTEM_INFO sys_info;
		GetSystemInfo(&sys_info);
		return sys_info.dwNumberOfProcessors;
#else
		return sysconf(_SC_NPROCESSORS_ONLN);
#endif
	}


	time_t simple_time::get_time_int() {
		return time(0);
	}

	struct tm simple_time::get_time_tm() {
		time_t current_time = time(0);
		struct tm *current_tm = localtime(&current_time);
		return *current_tm;
	}

	string simple_time::get_date_time_str(struct tm &t) {
		char stime_buf[32] = "";
		strftime(stime_buf, 32, "%Y-%m-%d %H:%M:%S", &t);
		return stime_buf;
	}

	string simple_time::get_date_time_str(time_t t) {
		struct tm *current_tm = localtime(&t);
		char stime_buf[32] = "";
		strftime(stime_buf, 32, "%Y-%m-%d %H:%M:%S", current_tm);
		return stime_buf;
	}

	string simple_time::get_date_str(struct tm &t) {
		char stime_buf[16] = "";
		strftime(stime_buf, 16, "%Y-%m-%d", &t);
		return stime_buf;
	}

	string simple_time::get_time_str(struct tm &t) {
		char stime_buf[16] = "";
		strftime(stime_buf, 16, "%H:%M:%S", &t);
		return stime_buf;
	}

	const char simple_time::MONTHDICT[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

	bool simple_time::get_date(time_t time_int, kook_data &D) {
		if (time_int <= 0) {
			return false;
		}
		time_int += CHTZ;  //加上中国时区时间
		int total_day = (int)(time_int / DAYSECOND) + 1; //得到从1970年来的天数
		int year = total_day / FOURYEARS * 4 + STARTYEAR;  //得到年
		int surplus_all_day = total_day % FOURYEARS;           //得到剩余的周年天数
		int month_count = 0;
		bool is_leap_year = false;
		//校验日期
		if (surplus_all_day == 0) {
			year -= 1;
			surplus_all_day = 365;
		}
		//得到年份与当前已过的天数
		if (surplus_all_day <= 365) {
		}
		else if (surplus_all_day <= 365 + 365) {
			year += 1;
			surplus_all_day -= 365;
		}
		else if (surplus_all_day <= 365 + 365 + 366) {
			year += 2;
			is_leap_year = true;
			surplus_all_day -= (365 + 365);
		}
		else {
			year += 3;
			surplus_all_day -= (365 + 365 + 366);
		}
		D.year = year;
		//得到月份和日期
		for (int i = 0; i < 12; i++) {
			month_count += MONTHDICT[i];
			if (is_leap_year && i == 1) {
				month_count += 1;
			}
			if (surplus_all_day - month_count > 0) {
				continue;
			}
			D.month = i + 1;
			D.day = ((D.month == 2 && is_leap_year) ? MONTHDICT[i] + 1 : MONTHDICT[i]) - (month_count - surplus_all_day);
			break;
		}
		return true;
	}

	bool simple_time::get_time(time_t time_int, kook_time &T) {
		if (time_int <= 0) {
			return false;
		}
		//加上中国时区时间
		time_int += CHTZ;
		//得到今天已过的秒数
		int gone_second = time_int % DAYSECOND;
		//得到今天的时
		T.hour = gone_second / HOURSECOND;
		//得到今天的分
		T.minute = (gone_second % HOURSECOND) / MINUTESECOND;
		//得到今天的秒
		T.second = gone_second % MINUTESECOND;
		return true;
	}

	bool simple_time::get_date_time(time_t time_int, kook_data_time &DT) {
		if (!get_date(time_int, DT.date)) {
			return false;
		}
		if (!get_time(time_int, DT.time)) {
			return false;
		}
		return true;
	}


	///////
	bool simple_rand_int::set_range(int min_n, int max_n) {
		if (min_n < 0 || max_n <= 0 || max_n > RAND_MAX ||
			min_n >= max_n) return false;
		min_number = min_n;
		max_number = max_n;
		return true;
	}
	int simple_rand_int::randint() {
		if (rand_count >= RAND_MAX) {
			// 当生成一定次数就重复初始化种子
			srand((unsigned int)time(0));
			rand_count = 0;
		}
		++rand_count;
		return (rand() % (max_number - min_number + 1)) + min_number;
	}
}
