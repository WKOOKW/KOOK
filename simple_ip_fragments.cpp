#include "simple_ip_fragments.h"

namespace kook {

	unsigned int simple_ip_addr_transform::sip_to_uint(string ip) {
		if (ip.empty()) {
			return 0;
		}
		return (unsigned int)inet_addr(ip.c_str());
	}
	string simple_ip_addr_transform::uint_to_sip(unsigned int ip) {
		if (0 == ip) {
			return "0.0.0.0";
		}
		in_addr addr;
		addr.s_addr = ip;
		return inet_ntoa(addr);
	}
	unsigned int simple_ip_addr_transform::nuint_to_huint(unsigned int ip) {
		return (unsigned int)ntohl((unsigned long)ip);
	}
	unsigned int simple_ip_addr_transform::huint_to_nuint(unsigned int ip) {
		return (unsigned int)htonl((unsigned long)ip);
	}
	unsigned short simple_ip_addr_transform::nushort_to_hushort(unsigned short port) {
		return ntohs(port);
	}
	unsigned short simple_ip_addr_transform::hushort_to_nushort(unsigned short port) {
		return htons(port);
	}

	///////////////////////////////////////////////
	bool simple_ip_fragments::set_max_size(long long size) {
		if (size <= 0) {
			return false;
		}
		max_size = size;
		return true;
	}

	bool simple_ip_fragments::set_ip_fragments(ip_fragments in_ip_fragments) {
		//检查
		if (ntohl(in_ip_fragments.start_target.ip) > ntohl(in_ip_fragments.end_target.ip) ||
			in_ip_fragments.start_target.port > in_ip_fragments.end_target.port) {
			return false;
		}
		//保存ip段
		s_ip_fragments = in_ip_fragments;
		//计数ip段的总数量
		all_count = (ntohl(in_ip_fragments.end_target.ip) - ntohl(in_ip_fragments.start_target.ip) + 1) * 
			(in_ip_fragments.end_target.port - in_ip_fragments.start_target.port + 1);
		//初始计数
		next_count = 0;
		return true;
	}

	bool simple_ip_fragments::next_ip_fragments(ip_fragments &out_ip_fragments) {
		if (0 == max_size || 0 == all_count) {
			return false;
		}
		//如果拆分完成也返回false
		if (next_count >= all_count) {
			return false;
		}
		//拆分ip段
		next_count += 1;
		out_ip_fragments = s_ip_fragments;
		out_ip_fragments.start_count = next_count;
		next_count += max_size - 1;
		if (next_count > all_count) {
			next_count = all_count;
		}
		out_ip_fragments.max_size = next_count;
		return true;
	}

	////////////////////////////////////////////////////
	bool foreach_ip_fragments::set_ip_fragments(ip_fragments in_ip_fragments) {
		//转换字节序
		in_ip_fragments.start_target.ip = ntohl(in_ip_fragments.start_target.ip);
		in_ip_fragments.end_target.ip = ntohl(in_ip_fragments.end_target.ip);
		//检查
		if (in_ip_fragments.start_target.ip > in_ip_fragments.end_target.ip ||
			in_ip_fragments.start_target.port > in_ip_fragments.end_target.port) {
			return false;
		}
		if (0 == in_ip_fragments.start_count || 0 == in_ip_fragments.max_size ||
			in_ip_fragments.start_count > in_ip_fragments.max_size) {
			return false;
		}
		s_ip_fragments = in_ip_fragments;
		return true;
	}

	bool foreach_ip_fragments::next_target(ip_target &out_ip_target) {
		if (0 == s_ip_fragments.max_size || 0 == s_ip_fragments.start_count) {
			return false;
		}
		// 遍历完成
		if (s_ip_fragments.start_count > s_ip_fragments.max_size) {
			return false;
		}
		out_ip_target = get_target(s_ip_fragments.start_count++);
		return true;
	}

	ip_target foreach_ip_fragments::get_target(long long next_count) {
		unsigned int current_ip = 0;
		unsigned int current_port = 0;
		ip_target temp_target;
		//计算计数器位置的ip+posr
		current_ip = (unsigned int)(next_count / (s_ip_fragments.end_target.port - s_ip_fragments.start_target.port + 1)); //计算ip的位置
		current_port = (unsigned int)(next_count % (s_ip_fragments.end_target.port - s_ip_fragments.start_target.port + 1));  //计算port位置
		if (0 == current_port) {
			temp_target.ip = s_ip_fragments.start_target.ip + current_ip - 1;
			temp_target.port = s_ip_fragments.end_target.port;
		}
		else {
			temp_target.ip = s_ip_fragments.start_target.ip + current_ip;
			temp_target.port = s_ip_fragments.start_target.port + (current_port - 1);
		}
		temp_target.ip = htonl(temp_target.ip); //转换为网络字节
		return temp_target;
	}

}