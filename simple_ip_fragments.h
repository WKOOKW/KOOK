#ifndef KOOK_SIMPLE_IP_FRAGMENTS_H_H
#define KOOK_SIMPLE_IP_FRAGMENTS_H_H

/*
IP段分割/遍历，利用IP与端口的连续性，只需要标记段的数量范围即可实现IP段的分割。通过取IP段数量范围的任务数字即可计算出具体的IP与端口
实现：不懂
*/

#ifdef _WIN32
// 忽略inet_addr等函数错误
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <arpa/inet.h>
#endif

#include <string>
using std::string;


namespace kook {

	// 网络地址转换，这里只做简单包装
	class simple_ip_addr_transform {
	public:
		// 将一个字符串ip转换为网络整数ip
		static unsigned int sip_to_uint(string ip);
		// 将一个网络整数ip转换为字符串ip
		static string uint_to_sip(unsigned int ip);
		// 将一个网络字序整数转换为主机字序整数，应用于无符号32位
		static unsigned int nuint_to_huint(unsigned int ip);
		// 将一个主机字序整数转换为网络字序整数，应用于无符号32位
		static unsigned int huint_to_nuint(unsigned int ip);
		// 将一个网络字序整数转换为主机字序整数，应用于无符号16位
		static unsigned short nushort_to_hushort(unsigned short port);
		// 将一个主机字序整数转换为网络字序整数，应用于无符号16位
		static unsigned short hushort_to_nushort(unsigned short port);
	};

	// IP目标
	class ip_target {
	public:
		ip_target() :ip(0), port(0) {}
		unsigned int ip;
		unsigned short port;
		bool operator== (ip_target &ips) {
			if (ip == ips.ip && port == ips.port) return true;
			return false;
		}
	};
	// IP段
	class ip_fragments {
	public:
		ip_fragments() :start_count(0), max_size(0) {}
		ip_target start_target;
		ip_target end_target;
		// 记录IP段起始的位置，当需要对一个大的IP段进行分割时，
		// 可以不修改ip_target的情况下，直接按start_count和max_size切割
		long long start_count;
		long long max_size;
		bool operator== (ip_fragments &ips) {
			if (start_target == ips.start_target && end_target == ips.end_target &&
				start_count == ips.start_count && max_size == ips.max_size) return true;
			return false;
		}
		void init() {
			// 通过start_target与end_target来初始化start_count与max_size，方便遍历IP段
			max_size = (ntohl(end_target.ip) - ntohl(start_target.ip) + 1) *
				(end_target.port - start_target.port + 1);
			start_count = 1;
		}
	};

	//简单的IP段拆分，这里的IP段拆分，并不是真正的将IP段分解，而只是记录拆分的长度和位置
	class simple_ip_fragments {
	public:
		simple_ip_fragments(long long size = 0): max_size(size), next_count(0), all_count(0){}
		//设置ip分段最大的数量
		bool set_max_size(long long size);
		//设置需要做ip分段的ip段结构
		bool set_ip_fragments(ip_fragments in_ip_fragments);
		//获取下一个已经分段好的ip段结构
		bool next_ip_fragments(ip_fragments &out_ip_fragments);
		//获取总包数
		long long get_sum() {
			return all_count;
		}
	protected:
		//单段最大的port数量
		long long max_size;
		//拆分计数
		long long next_count;
		//总的数据包计数
		long long all_count;
		//要拆分的ip段结构
		ip_fragments s_ip_fragments;
	};

	//遍历IP段，将IP段以IP+PORT一个个遍历出来
	class foreach_ip_fragments {
	public:
		//设置需要做ip分段遍历的ip段结构
		bool set_ip_fragments(ip_fragments in_ip_fragments);
		//遍历ip段，遍历完或遍历失败返回false
		bool next_target(ip_target &out_ip_target);
		//通过next_count计数来计算出target
		ip_target get_target(long long next_count);
	protected:
		ip_fragments s_ip_fragments;
	};

}

#endif