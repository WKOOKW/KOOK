#ifndef KOOK_SMALL_LOG_H_H
#define KOOK_SMALL_LOG_H_H

/*
简单的日志记录模块
实现：不懂
*/

//  一个轻量级的日志记录对象
//  包含日志内容有：(记录的日期，记录的时间，记录的文件，记录的函数，记录的行数，记录的内容)
//  日志等级：(info,debug,warning,error,urgency)对应的数字是：(0.1.2.3.4.5)

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <string>
using std::string;
#include <fstream>
using std::ofstream;
#include <sstream>
using std::ostringstream;

#include "share_resource.h"

namespace kook {

	//  一些日志缓存空间定义
#define SLOG_BUF_SIZE_64   64
#define SLOG_BUF_SIZE_128   128
#define SLOG_BUF_SIZE_256   256
#define SLOG_BUF_SIZE_512   512
#define SLOG_BUF_SIZE_B   SLOG_BUF_SIZE_128

//  日志信息宏
#define RECORD_SCENE_FILE   __FILE__
#define RECORD_SCENE_FUNC   __FUNCTION__
#define RECORD_SCENE_LINE   __LINE__

	class small_log {
	protected:
		unsigned int small_log_info;       // 指定日志显示的内容，见LOGINFO
		unsigned int small_log_level;      // 指定日志显示的起始级别，见LOGLEVEL
		unsigned int small_log_mode;       // 指定日志输出模式(0输出到std，1输出到文件，2同时输出到std和文件)，见LOGMODE
		string log_file_path;         // 指定日志文件路径
		ofstream out_file;            // 文件写入对象
		bool is_record;               // 是否启动日志记录
	public:
		enum SMALLLOGINFO {           // 日志记录的内容
			SMALLLOG_ALL = 31,
			SMALLLOG_DATE = 16,
			SMALLLOG_TIME = 8,
			SMALLLOG_FILE = 4,
			SMALLLOG_FUNCTION = 2,
			SMALLLOG_LINE = 1
		};
		enum SMALLLOGLEVEL {      // 日志记录的级别
			SMALLLOG_DEBUG,
			SMALLLOG_INFO,
			SMALLLOG_WARNING,     // 从警告开始会向stderr输出
			SMALLLOG_ERROR,
			SMALLLOG_URGENCY
		};
		enum SMALLLOGMODE {       // 日志记录的模式
			SMALLLOG_OUT_STD,     // 输出到stdout，注意：从警告开始会向stderr输出
			SMALLLOG_OUT_FILE,    // 输出到文件
			SMALLLOG_OUT_ALL      // 同时输出到std和文件
		};
		enum SMALLLOGSHOW {
			SMALLLOG_LOW = SMALLLOG_TIME | SMALLLOG_FILE | SMALLLOG_LINE,
			SMALLLOG_MEDIUM = SMALLLOG_TIME | SMALLLOG_FILE | SMALLLOG_FUNCTION | SMALLLOG_LINE,
			SMALLLOG_HIGH = SMALLLOG_ALL
		};

		small_log() : small_log_info(SMALLLOG_ALL), small_log_level(SMALLLOG_DEBUG), small_log_mode(SMALLLOG_OUT_STD), is_record(true) {}
		~small_log() {
			if (out_file.is_open()) {
				out_file.close();
			}
		}
		// 设置日志内容，包含(记录的日期，记录的时间，记录的文件，记录的函数，记录的行数)
		// join=true表示将内容加入日志，如果为false表示将内容从日志中减去
		bool set_show_log_info(SMALLLOGINFO info, bool join = true);
		unsigned int get_show_log_info();
		// 设置日志显示等级，即从什么级别开始记录，包含((info,debug,warning,error,urgency)对应的数字是：(0.1.2.3.4.5))
		bool set_show_log_level(SMALLLOGLEVEL level);
		unsigned int get_show_log_level();
		// 记录日志的模式，即将日志输出到屏幕还是文件，或都输出，对应数字(0,1,2)
		// 如果输出到文件，由log_file_path来指定文件路径
		bool set_show_log_mode(SMALLLOGMODE mode, string log_file_path = "");
		unsigned int get_show_log_mode();
		// 输出日志，level指定日志的级别，data指定日志的具体内容,log_file,log_func,log_line,这些参数只是记录的位置信息，只需要填写固定的宏即可，content用于单独指定日志内容信息
		bool log(SMALLLOGLEVEL level, string data, string log_file, string log_func, unsigned int log_line, unsigned int content = 0);
		// 刷新日志文件缓冲区，当日志写入文件时，为了防止日志丢失过多，可以在写入日志后，刷新缓冲，让日志真正的写入文件中
		// 注意：如果不是特别重要的日志，建议不要频繁刷新，会增加io负担
		bool log_file_io_flush();
		// 获取组合后的日志内容，此函数会将日志信息直接返回并不输出
		bool get_log_str(SMALLLOGLEVEL level, string data, string &log_data, string log_file, string log_func, unsigned int log_line, unsigned int content = 0);
		// 判断日志记录是否开启
		bool is_record_log();
		// 停止日志记录
		bool stop_record_log();
		// 启动日志记录，默认为启动
		bool start_record_log();
		// 迁移配置，用于同步日志配置，即将slog对象的配置迁移到本日志对象
		bool migration_config(small_log &slog);

	protected:
		// 获取级别对应的显示字符串
		bool get_level_str(SMALLLOGLEVEL level, string &level_str);
		// 获取显示内容信息，如(记录的日期，记录的时间，记录的文件，记录的函数，记录的行数)
		bool get_show_info_str(string &log_info, string log_file, string log_func, unsigned int log_line, unsigned int content = 0);
		// 将日志信息写入输出设备
		bool write_log(string &log_data, SMALLLOGLEVEL level);
	};

	//  全局宏
#define small_log_debug(slog,doc) slog.log(slog.SMALLLOG_DEBUG,doc,RECORD_SCENE_FILE,RECORD_SCENE_FUNC,RECORD_SCENE_LINE)
#define small_log_info(slog,doc) slog.log(slog.SMALLLOG_INFO,doc,RECORD_SCENE_FILE,RECORD_SCENE_FUNC,RECORD_SCENE_LINE)
#define small_log_warning(slog,doc) slog.log(slog.SMALLLOG_WARNING,doc,RECORD_SCENE_FILE,RECORD_SCENE_FUNC,RECORD_SCENE_LINE)
#define small_log_error(slog,doc) slog.log(slog.SMALLLOG_ERROR,doc,RECORD_SCENE_FILE,RECORD_SCENE_FUNC,RECORD_SCENE_LINE)
#define small_log_urgency(slog,doc) slog.log(slog.SMALLLOG_URGENCY,doc,RECORD_SCENE_FILE,RECORD_SCENE_FUNC,RECORD_SCENE_LINE)

}

#endif