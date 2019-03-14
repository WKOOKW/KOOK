#ifndef KOOK_SIMPLE_ZLIB_H_H
#define KOOK_SIMPLE_ZLIB_H_H

/*
zlib压缩/解压
实现：不懂
*/

#define ZLIB_CONST
#include "zlib.h"
#ifdef _WIN32
#pragma comment(lib,"zlib.lib")
#endif
#include <string>
using std::string;
#include "small_log.h"

namespace kook {
	// 简化zlib，使用string做为缓冲区，linux编译依赖zlib库-lz

	// 分布式架构通信中，80%上的数据少于16KB，所以这里的固定压缩缓存定为16KB
	// 后续可改为动态缓存空间，根据要压缩的数据量来动态的调整缓存空间，达到压缩的速度与空间利用最优化
	const unsigned int ZLIB_MIDDLE_BUF_SIZE = 16 * 1024;

	class simple_compress {
	public:
		// 数据压缩，压缩的级别默认保持默认级别
		// 内部使用zlib数据流压缩模式，通过数据流分块压缩，可减少内存消耗
		string compress(string &data, int level = Z_DEFAULT_COMPRESSION);
		small_log slog;
		string operator()(string &data, int level = Z_DEFAULT_COMPRESSION);
	};

	class simple_uncompress {
	public:
		// 数据解压缩
		// 内部使用zlib数据流压缩模式，通过数据流分块压缩，可减少内存消耗
		string uncompress(string &data);
		small_log slog;
		string operator()(string &data);
	};

	// 全局函数，压缩
	string simple_compress_ex(string &data, int level = Z_DEFAULT_COMPRESSION);
	// 全局函数，解压
	string simple_uncompress_ex(string &data);

}

#endif