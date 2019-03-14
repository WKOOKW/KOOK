//一个轻量级的日志记录对象
//包含日志内容有：(记录的日期，记录的时间，记录的文件，记录的函数，记录的行数，记录的内容)
//日志等级：(info,debug,warning,error,urgency)对应的数字是：(0.1.2.3.4.5)

#include "small_log.h"

namespace kook {

	bool small_log::set_show_log_info(SMALLLOGINFO info, bool join) {
		if (info < 0 || info > SMALLLOG_ALL) {
			return false;
		}
		if (join) {
			small_log_info |= info;
		}
		else {
			small_log_info &= ~info;
		}
		return true;
	}

	unsigned int small_log::get_show_log_info() {
		return small_log_info;
	}

	bool small_log::set_show_log_level(SMALLLOGLEVEL level) {
		if (level <0 || level > 5) {
			return false;
		}
		small_log_level = level;
		return true;
	}

	unsigned int small_log::get_show_log_level() {
		return small_log_level;
	}

	bool small_log::set_show_log_mode(SMALLLOGMODE mode, string log_file_path) {
		switch (mode) {
		case SMALLLOG_OUT_ALL:
			small_log_mode = SMALLLOG_OUT_ALL;
			this->log_file_path = log_file_path;
			break;
		case SMALLLOG_OUT_FILE:
			small_log_mode = SMALLLOG_OUT_FILE;
			this->log_file_path = log_file_path;
			break;
		case SMALLLOG_OUT_STD:
			small_log_mode = SMALLLOG_OUT_STD;
			break;
		default:
			return false;
		}
		return true;
	}

	unsigned int small_log::get_show_log_mode() {
		return small_log_mode;
	}

	bool small_log::is_record_log() {
		return is_record;
	}

	bool small_log::stop_record_log() {
		is_record = false;
		if (out_file.is_open()) {
			out_file.close();
		}
		return true;
	}

	bool small_log::start_record_log() {
		is_record = true;
		return true;
	}

	bool small_log::get_log_str(SMALLLOGLEVEL level, string data, string &log_data, string log_file, string log_func, unsigned int log_line, unsigned int content) {
		if (!is_record || (unsigned int)level < this->small_log_level) {
			return true;
		}
		string log_level_str;
		if (!get_level_str(level, log_level_str)) {
			return false;
		}
		get_show_info_str(log_data, log_file, log_func, log_line, content);
		log_data += (log_level_str + "|" + data);
		return true;
	}

	bool small_log::log(SMALLLOGLEVEL level, string data, string log_file, string log_func, unsigned int log_line, unsigned int content) {
		if (!is_record || (unsigned int)level < this->small_log_level) {
			return true;
		}
		string log_level_str;
		string log_info_str;
		if (!get_level_str(level, log_level_str)) {
			return false;
		}
		get_show_info_str(log_info_str, log_file, log_func, log_line, content);
		log_info_str += (log_level_str + "|" + data);
		return write_log(log_info_str, level);
	}

	bool small_log::log_file_io_flush() {
		if (!is_record || !out_file.is_open()) return false;
		out_file.flush();
		return true;
	}

	bool small_log::get_level_str(SMALLLOGLEVEL level, string &level_str) {
		switch (level) {
		case SMALLLOG_INFO:
			level_str = "INFO";
			break;
		case SMALLLOG_DEBUG:
			level_str = "DEBUG";
			break;
		case SMALLLOG_WARNING:
			level_str = "WARNING";
			break;
		case SMALLLOG_ERROR:
			level_str = "ERROR";
			break;
		case SMALLLOG_URGENCY:
			level_str = "URGENCY";
			break;
		default:
			return false;
		}
		return true;
	}

	bool small_log::get_show_info_str(string &log_info, string log_file, string log_func, unsigned int log_line, unsigned int content) {
		ostringstream temp_log;
		/*time_t time_int = simple_time::get_time_int();
		kook_data_time DT;*/
		struct tm T= simple_time::get_time_tm();
		unsigned int temp_info = this->small_log_info;
		if (content > 0) {
			temp_info = content;
		}
		if (temp_info & SMALLLOG_DATE) {
			/*simple_time::get_date(time_int, DT.date);
			temp_log << DT.date.year << "-" << DT.date.month << "-" << DT.date.day << " ";*/
			temp_log << simple_time::get_date_str(T) << " ";
		}
		if (temp_info & SMALLLOG_TIME) {
			/*simple_time::get_time(time_int, DT.time);
			temp_log << DT.time.hour << ":" << DT.time.minute << ":" << DT.time.second << " ";*/
			temp_log << simple_time::get_time_str(T) << " ";
		}
		if (temp_info & SMALLLOG_FILE) {
			temp_log << log_file << "|";
		}
		if (temp_info & SMALLLOG_FUNCTION) {
			temp_log << log_func << "|";
		}
		if (temp_info & SMALLLOG_LINE) {
			temp_log << log_line << "|";
		}
		log_info = temp_log.str();
		return true;
	}

	bool small_log::write_log(string &log_data, SMALLLOGLEVEL level) {
		if (0 == small_log_mode && out_file.is_open()) {
			out_file.close();
		}
		if (0 != small_log_mode && !out_file.is_open()) {
			out_file.open(log_file_path.c_str(), std::ios::out | std::ios::app | std::ios::binary);
			if (!out_file.is_open()) {
				return false;
			}
		}
		if (0 == small_log_mode || 2 == small_log_mode) {
			if (level >= SMALLLOG_WARNING) {
				cerr << log_data << endl;
			}
			else {
				cout << log_data << endl;
			}
		}
		if (1 == small_log_mode || 2 == small_log_mode) {
			out_file.write((log_data + "\n").c_str(), log_data.size() + 1);
		}
		return true;
	}

	bool small_log::migration_config(small_log &slog) {
		this->small_log_info = slog.small_log_info;
		this->small_log_level = slog.small_log_level;
		this->small_log_mode = slog.small_log_mode;
		this->log_file_path = slog.log_file_path;
		this->is_record = slog.is_record;
		return true;
	}

}