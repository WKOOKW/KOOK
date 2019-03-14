#include "simple_zlib.h"

namespace kook {

	string simple_compress::compress(string &data, int level) {
		if (data.empty()) {
			small_log_debug(slog, "compress data null");
			return "";
		}
		int ret, flush;
		unsigned int have;
		z_stream zlib_strm;
		// 默认每次最大的压缩缓存区，正常情况下越大越好，但在通信中，有很多小协议数据，所以看情况定义
		unsigned char out_data[ZLIB_MIDDLE_BUF_SIZE];
		char log_buf[SLOG_BUF_SIZE_B];
		string zlib_data;
		// zlib结构填充，zalloc和zfree为内存分配释放函数，如果设置为Z_NULL表示使用默认的内存分配函数
		// 默认内存分配函数为malloc和free
		zlib_strm.zalloc = Z_NULL;
		zlib_strm.zfree = Z_NULL;
		zlib_strm.opaque = Z_NULL;
		// 初始化内部流状态
		ret = deflateInit(&zlib_strm, level);
		if (ret != Z_OK) {
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "compress stream init fail, error code: %d", ret);
			small_log_warning(slog, log_buf);
			return "";
		}
		// 设置源数据流，要压缩的数据
		zlib_strm.avail_in = data.size();
		// Z_FINISH表式数据流读取结果，Z_NO_FLUSH表式数据流读取未结束，需要继续读取
		// 在这里所有数据都以经在data中了，所以这里不需要再循环处理
		flush = Z_FINISH;
		zlib_strm.next_in = (const Byte *)data.c_str();
		// 循环调用deflate来压缩数据流，这里对数据流分块压缩，减少对内存的消耗
		do {
			zlib_strm.avail_out = ZLIB_MIDDLE_BUF_SIZE;
			zlib_strm.next_out = out_data;
			ret = deflate(&zlib_strm, flush);
			if (ret == Z_STREAM_ERROR) {
				STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "compress stream error, compress break, error code: %d", ret);
				small_log_warning(slog, log_buf);
				break;
			}
			have = ZLIB_MIDDLE_BUF_SIZE - zlib_strm.avail_out;
			// 保存已压缩数据
			zlib_data.append((const char *)out_data, have);
		} while (zlib_strm.avail_out == 0);
		if (zlib_strm.avail_in != 0 || ret != Z_STREAM_END) {
			// 还在数据未被压缩，不正常结束，这里先不干预压缩数据返回，但需要注意压缩数据可能会不完整
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "compress stream exception break, error code: %d", ret);
			small_log_warning(slog, log_buf);
		}
		ret = deflateEnd(&zlib_strm);
		if (ret != Z_OK) {
			// 释放流失败，可能会导致内存泄露，这里先不干预压缩数据返回，但需要注意压缩数据可能会不完整
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "compress stream free exception, error code: %d", ret);
			small_log_warning(slog, log_buf);
		}
		return zlib_data;
	}

	string simple_compress::operator()(string &data, int level) {
		return this->compress(data, level);
	}

	//////////////////////////////////////////////////
	string simple_uncompress::uncompress(string &data) {
		if (data.empty()) {
			small_log_debug(slog, "uncompress data null");
			return "";
		}
		int ret, flush;
		unsigned int have;
		z_stream zlib_strm;
		// 默认每次最大的解压缓存区，正常情况下越大越好，但在通信中，有很多小协议数据，所以看情况定义
		unsigned char out_data[ZLIB_MIDDLE_BUF_SIZE];
		char log_buf[SLOG_BUF_SIZE_B];
		string zlib_data;
		// zlib结构填充，zalloc和zfree为内存分配释放函数，如果设置为Z_NULL表示使用默认的内存分配函数
		// 默认内存分配函数为malloc和free
		zlib_strm.zalloc = Z_NULL;
		zlib_strm.zfree = Z_NULL;
		zlib_strm.opaque = Z_NULL;
		// 初始化内部流状态
		ret = inflateInit(&zlib_strm);
		if (ret != Z_OK) {
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "uncompress stream init fail, error code: %d", ret);
			small_log_warning(slog, log_buf);
			return "";
		}
		// 设置源数据流，要解压的数据
		zlib_strm.avail_in = data.size();
		// Z_FINISH表式数据流读取结果，Z_NO_FLUSH表式数据流读取未结束，需要继续读取
		// 在这里所有数据都以经在data中了，所以这里不需要再循环处理
		flush = Z_FINISH;
		zlib_strm.next_in = (const Byte *)data.c_str();
		// 循环调用inflate来解压数据流，这里对数据流分块压缩，减少对内存的消耗
		do {
			zlib_strm.avail_out = ZLIB_MIDDLE_BUF_SIZE;
			zlib_strm.next_out = out_data;
			ret = inflate(&zlib_strm, flush);
			if (ret == Z_STREAM_ERROR) {
				STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "uncompress stream error, uncompress break, error code: %d", ret);
				small_log_warning(slog, log_buf);
				break;
			}
			have = ZLIB_MIDDLE_BUF_SIZE - zlib_strm.avail_out;
			// 保存已解压数据
			zlib_data.append((const char *)out_data, have);
		} while (zlib_strm.avail_out == 0);
		if (zlib_strm.avail_in != 0 || ret != Z_STREAM_END) {
			// 还在数据未被解压，不正常结束，这里先不干预解压数据返回，但需要注意解压数据可能会不完整
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "uncompress stream exception break, error code: %d", ret);
			small_log_warning(slog, log_buf);
		}
		ret = inflateEnd(&zlib_strm);
		if (ret != Z_OK) {
			// 释放流失败，可能会导致内存泄露，这里先不干预解压数据返回，但需要注意解压数据可能会不完整
			STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "uncompress stream free exception, error code: %d", ret);
			small_log_warning(slog, log_buf);
		}
		return zlib_data;
	}

	string simple_uncompress::operator()(string &data) {
		return this->uncompress(data);
	}

	// 全局函数 //////////////////////////
	string simple_compress_ex(string &data, int level) {
		simple_compress compress_ex;
		return compress_ex.compress(data, level);
	}
	string simple_uncompress_ex(string &data) {
		simple_uncompress uncompress_ex;
		return uncompress_ex.uncompress(data);
	}

}