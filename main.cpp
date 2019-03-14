/*
kook公共库主要包含一些日常用到的小模块，可在Windows和Linux平台使用，测试平台(Windows10和CentOS7)
注意：这里不确保代码无BUG，使用时请测试和检查
测试：不懂
*/

// X509_cert证书获取测试实例///////////////////////////////////////
// Linux g++ 测试编译命令：g++ X509_cert.cpp hex.cpp main.cpp -lssl -lcrypto -o test
// 注意：依赖hex与openssl
//#include "X509_cert.h"
//
//#ifndef _WIN32
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <errno.h>
//#include <unistd.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#endif
//#include <iostream>
//using namespace std;
//
//void close_sock(int s) {
//	if (s <= 2) return;
//#ifdef WIN32
//	closesocket(s);
//#else
//	close(s);
//#endif
//}
//
//int main() {
//	kook::init_base_socket base_s;
//	int sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (-1 == sock) return 0;
//	SSL *ssl_sock = SSL_new(base_s.get_ctx());
//	if (0 == ssl_sock) {
//		close_sock(sock);
//		return 0;
//	}
//	// 连接
//	sockaddr_in addr;
//	addr.sin_addr.s_addr = inet_addr("14.215.177.38");
//	addr.sin_port = htons(443);
//	addr.sin_family = AF_INET;
//	int ret = connect(sock, (const sockaddr *)&addr, sizeof(addr));
//	if (-1 == ret) {
//		SSL_free(ssl_sock);
//		close_sock(sock);
//		return 0;
//	}
//	// 邦定ssl
//	if (SSL_set_fd(ssl_sock, sock) == 0) {
//		SSL_free(ssl_sock);
//		close_sock(sock);
//		return 0;
//	}
//	// ssl连接
//	if (SSL_connect(ssl_sock) != 1) {
//		SSL_free(ssl_sock);
//		close_sock(sock);
//		return 0;
//	}
//	// 获取证书
//	kook::ssl_X509_cert cert(ssl_sock);
//	kook::X509_cert_s cert_info = cert.get_X509_Cert();
//	cout << cert_info.version << endl;
//	cout << cert_info.issuer << endl;
//	cout << cert_info.owner << endl;
//	cout << cert_info.dns << endl;
//	cout << cert_info.validity_time << endl;
//	return 0;
//}


// base64编码测试实例////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ base64.cpp main.cpp -o test
//#include "base64.h"
//#include <iostream>
//int main()
//{
//kook::base64 ba;
//string rrr , ccc;
//char a[10] = {0x3D,0x41,0x00,0x00,0x3D,0x52,0x00,0x00,0x44,0x8E};
//ba.base64_input(a , 10);
//rrr = ba.base64_get_encode();
//std::cout<<rrr<<std::endl;
//ba.base64_input(rrr.c_str() , rrr.size());
//ccc = ba.base64_get_decode();
//ccc;
//return 0;
//}


// md5校验测试实例////////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ md5.cpp main.cpp -o test
//#include <iostream>
//#include "md5.h"
//
//using namespace std;
//
//int main(int argc, char *argv[])
//{
//	string text = "jfdklsjkgjdskfksj4g5r6w42vgfsd1g5s64g5d1s5b4e545d6s4f5ds64f56ds464v4cx54";
//	kook::MD5 m5(text);
//	cout << "a: " << m5.hexdigest() << endl;
//	text = "ikgjriowegr7eb489d4h5vdf4h86d74h5b4df86f4gfd4564h56df4db";
//	cout << "b: " << kook::md5(text) << endl;
//	return 0;
//}


// url编码测试实例/////////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ url.cpp main.cpp -o test
//#include <iostream>
//#include "url.h"
//using namespace std;
//int main()
//{
//	kook::URL url;
//	string a;
//	char *p = "http://www.baidu.com/aaa/index.php?id=1&c=你好";
//	url.url_input(p, strlen(p));
//	a = url.url_get_encode(kook::URL_SAFETY_CHSR1);
//	cout << a << endl;
//
//	url.url_input(a.c_str(), a.size());
//	cout << url.url_get_decode() << endl;
//	return 0;
//}

// zlib压缩测试实例////////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ simple_zlib.cpp share_resource.cpp small_log.cpp main.cpp -lz -o test
// 注意：依赖share_resource、small_log、zlib
//#include <iostream>
//#include <string>
//#include "simple_zlib.h"
//using namespace std;
//int main()
//{
//	string text, result;
//	text = "jdkslg4d5s64f5ds64f12sd345ew41v5d1s564g545ds1bv1sd64fd5s4f";
//	result = kook::simple_compress_ex(text);
//	cout << "compress size: " << result.size() << endl;
//	result = kook::simple_uncompress_ex(result);
//	cout << "uncompress: " << result << endl;
//	return 0;
//}

// 随机数据生成测试实例////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ radom_integer_iterator.cpp main.cpp -o test
//#include "radom_integer_iterator.h"
//#include <iostream>
//using std::cout;
//using std::endl;
//int main() {
//	kook::radom_integer_iterator rii;
//	unsigned long testc = 0;
//	// 设定随机数序列范围
//	rii.reset_n(50);
//	while (true)
//	{
//		// 每次生成一个随机数，如果返回0表示一个循环生成完毕
//		testc = rii.next();
//		if (testc == 0) {
//			break;
//		}
//		cout << testc << " ";
//	}
//	return 0;
//}

// hex编码测试实例///////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ hex.cpp main.cpp -o test
//#include <iostream>
//#include "hex.h"
//using namespace std;
//int main(){
//	string text = "afds41f5d6s412fds341f5ds1f2ds345efds";
//	string result;
//	// 编码
//	kook::HEX h16(text.c_str(), text.size());
//	result = h16.get_to_hex();
//	cout << "hex: " << result << endl;
//	// 解码
//	h16.hex_input(result.c_str(), result.size());
//	result = h16.get_to_text();
//	cout << "sou: " << result << endl;
//	return 0;
//}

// share_resource公共模块测试实例//////////////////////////////////
// Linux g++ 测试编译命令：g++ share_resource.cpp main.cpp -o test
//#include <iostream>
//#include "share_resource.h"
//using namespace std;
//int main() {
//	cout << "cpu size: " << kook::share_resource::get_cpu_core_number() << endl;
//	kook::share_resource::sleep();
//	kook::simple_rand_int rint;
//	cout << "randint: " << rint.randint() << endl;
//	// 当前系统时间
//	time_t t = kook::simple_time::get_time_int();
//	cout << "time: " << kook::simple_time::get_date_time_str(t) << endl;
//	return 0;
//}

// 日志模块测试实例//////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ small_log.cpp share_resource.cpp main.cpp -o test
//#include "small_log.h"
//int main(){
//	string text = "test log out";
//	// 简单的日志模块，可并发输出日志，参见thread_pool模块
//	kook::small_log slog;
//	// 输出日志到控制台
//	small_log_debug(slog, text);
//	small_log_error(slog, text);
//	slog.log(slog.SMALLLOG_INFO, text, RECORD_SCENE_FILE, RECORD_SCENE_FUNC, RECORD_SCENE_LINE);
//	char log_buf[SLOG_BUF_SIZE_B];
//	STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "test int: %d, log out", 123);
//	small_log_debug(slog, log_buf);
//	// 输出日志到文件和控制台
//	string text_file = "test log out file";
//	slog.set_show_log_mode(slog.SMALLLOG_OUT_ALL, "test_log.log");
//	small_log_debug(slog, text_file);
//	small_log_error(slog, text_file);
//	// 刷新日志文件缓冲，快速将日志写入文件
//	slog.log_file_io_flush();
//	return 0;
//}

// 线程池测试实例//////////////////////////////////////////
// Linux g++ 测试编译命令：g++ thread_pool.cpp small_log.cpp share_resource.cpp main.cpp -lpthread -o test
// 注意：依赖small_log、share_resource、pthread
#include <iostream>
using namespace std;
#include "thread_pool.h"

// 线程池任务工作函数，这里只需要自己编写任务处理方法即可，线程的控制，任务分配，状态记录等都是由管理对象自动完成
// 用户只需要专心处理业务即可
bool thread_work_test(kook::state_signal &signal, void* task_data, void* out_result, kook::small_log &slog) {
	// 对任务队列中的整数加1
	int *task_int = (int *)task_data;
	int *result_int = (int *)out_result;
	char log_buf[SLOG_BUF_SIZE_B];
	STRFORMAT(log_buf, SLOG_BUF_SIZE_B, "int: %d + 1 = %d", (*task_int), (*task_int) + 1);
	small_log_debug(slog, log_buf);
	// 保存结果，用于传递到结果队列
	(*result_int) += 1;
	return true;
}

int main() {
	// 设置工作函数是必须的，否则任务不会被执行，也可以后面通过reset_work_call来设置
	// 管理对象是一个模板对象，需要指定任务队列与结果队列的数据类型
	kook::thread_pool_manage<int, int> test_thread_pool(thread_work_test);
	test_thread_pool.set_max_thread_size(10);
	// 重置工作函数
	// test_thread_pool.reset_work_call(thread_work_test);
	// 增加任务，也可以启动后增加
	test_thread_pool.add_task(10);
	test_thread_pool.add_task(11);
	test_thread_pool.add_task(19);
	test_thread_pool.add_task(11111);
	// 启动线程池
	test_thread_pool.start_thread_pool();
	// 可以通过多种形式来判断任务是否被执行完
	// 简单的情况可以直接通过任务计数来判断，但在复杂的情况下不能真实的反应任务情况
	while (test_thread_pool.get_work_count() < 4)
	{
		kook::share_resource::sleep();
		continue;
	}
	// 通过任务队列是否空和当前是否有线程工作来判断，当任务队列已经没有任务，并且所有线程都工作完成，说明所有任务执行完成
	while (!test_thread_pool.is_task() || test_thread_pool.get_thread_work_size() != 0)
	{
		kook::share_resource::sleep();
		continue;
	}
	// 查看线程池运行的线程数量，他与工作的线程数量不同，这里的数量是启动的线程数量，有一些线程可能没有工作，处于休眠中
	unsigned int tsize = test_thread_pool.get_run_thread_size();
	cout << "current run thread size: " << tsize << endl;
	// 运行中增加任务
	test_thread_pool.add_task(222);
	test_thread_pool.add_task(123);
	// 没有进行状态操作，此时的状态应该是0(运行中)
	unsigned int state = test_thread_pool.get_thread_run_state();
	cout << "current state: " << state << endl;
	// 停止任务，注意：如果任务执行中不响应停止，那么就只能等待任务执行完成后才能停止，所以状态的操作都是被动的
	test_thread_pool.stop();
	kook::share_resource::sleep();
	state = test_thread_pool.get_thread_run_state();
	cout << "current state: " << state << endl;
	// 停止后再次运行
	test_thread_pool.add_task(333);
	test_thread_pool.add_task(555);
	test_thread_pool.run();
	kook::share_resource::sleep();
	state = test_thread_pool.get_thread_run_state();
	cout << "current state: " << state << endl;
	// 也可以运行中调整线程池线程的数量
	test_thread_pool.set_max_thread_size(5);
	kook::share_resource::sleep();
	tsize = test_thread_pool.get_run_thread_size();
	cout << "current run thread size: " << tsize << endl;
	// 查看结果数量，也可以获取结果去处理，请看接口说明
	unsigned int rsize = test_thread_pool.get_result_size();
	cout << "current result size: " << rsize << endl;
	// 结束线程池，线程池结束后，如果想再次工作，需要调用start_thread_pool来启动线程池
	test_thread_pool.exit();
	while (test_thread_pool.get_thread_run_state() != kook::thread_pool_state::EXIT)
	{
		kook::share_resource::sleep();
		continue;
	}
	state = test_thread_pool.get_thread_run_state();
	cout << "current state: " << state << endl;
	return 0;
}

// kook_string_function测试实例/////////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ kook_string_function.cpp main.cpp -o test
//#include <iostream>
//#include <string>
//using namespace std;
//#include "kook_string_function.h"
//
//int main() {
//	string test_text = "jfkdls|5415fd|gkls|564|455";
//	const char *test_text_pt = test_text.c_str();
//	char test_buf[64];
//	// 循环的截取字符串
//	while ((test_text_pt = kook::kook_string_function::iteration_str(test_text_pt, '|', test_buf, 64)))
//	{
//		cout << "iteration str: " << test_buf << endl;
//	}
//	// 单次截取字符串
//	test_text_pt = test_text.c_str();
//	kook::kook_string_function::interception_str(test_text_pt, '|', test_buf, 64);
//	cout << "interception str: " << test_buf << endl;
//	return 0;
//}

// simple_ip_fragments IP段分割测试实例//////////////////////////////////////////////
// Linux g++ 测试编译命令：g++ simple_ip_fragments.cpp main.cpp -o test
//#include <iostream>
//#include <string>
//using namespace std;
//#include "simple_ip_fragments.h"
//int main() {
//	// 测试IP段为空的情况，会生成一个IP和PORT为0的目标
//	/*kook::ip_fragments test_ip_fragments;
//	test_ip_fragments.init();
//	kook::foreach_ip_fragments test_foreach;
//	kook::ip_target test_target;
//	test_foreach.set_ip_fragments(test_ip_fragments);
//	test_foreach.next_target(test_target);
//	test_target;*/
//	// 测试一个C段遍历
//	/*kook::ip_fragments test_ip_fragments;
//	test_ip_fragments.start_target.ip = kook::simple_ip_addr_transform::sip_to_uint("192.168.11.1");
//	test_ip_fragments.start_target.port = 1;
//	test_ip_fragments.end_target.ip = kook::simple_ip_addr_transform::sip_to_uint("192.168.11.255");
//	test_ip_fragments.end_target.port = 65535;
//	test_ip_fragments.init();
//	kook::foreach_ip_fragments test_foreach;
//	kook::ip_target test_target;
//	test_foreach.set_ip_fragments(test_ip_fragments);
//	while (test_foreach.next_target(test_target))
//	{
//		cout << "ip: " << kook::simple_ip_addr_transform::uint_to_sip(test_target.ip) << "   port: " << test_target.port << endl;
//	};*/
//	// 切分段再遍历
//	kook::ip_fragments test_ip_fragments;
//	test_ip_fragments.start_target.ip = kook::simple_ip_addr_transform::sip_to_uint("192.168.11.1");
//	test_ip_fragments.start_target.port = 1;
//	test_ip_fragments.end_target.ip = kook::simple_ip_addr_transform::sip_to_uint("192.168.11.255");
//	test_ip_fragments.end_target.port = 65535;
//	kook::ip_fragments test_ip_fragments_for;
//	kook::simple_ip_fragments test_segmentation(200000);
//	test_segmentation.set_ip_fragments(test_ip_fragments);
//	while (test_segmentation.next_ip_fragments(test_ip_fragments_for))
//	{
//		kook::foreach_ip_fragments test_foreach;
//		kook::ip_target test_target;
//		test_foreach.set_ip_fragments(test_ip_fragments_for);
//		while (test_foreach.next_target(test_target))
//		{
//			cout << "ip: " << kook::simple_ip_addr_transform::uint_to_sip(test_target.ip) << "   port: " << test_target.port << endl;
//		};
//	}
//	return 0;
//}