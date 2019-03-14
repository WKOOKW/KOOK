#ifndef KOOK_X509_CERT_H_H
#define KOOK_X509_CERT_H_H

/*
通过OpenSSL获取X509证书信息
实现：不懂
*/

#ifdef _WIN32
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#include<string>
using std::string;

// 请使用第三方库<openssl>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/x509v3.h>
#ifdef _WIN32
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")
#endif

// 依赖kook/hex
#include "hex.h"


#ifndef KOOK_SHARE_RESOURCE_H_H
#ifdef _WIN32
#define STRFORMAT sprintf_s
#else
#define STRFORMAT snprintf
#endif
#endif // KOOK_SHARE_RESOURCE_H_H


namespace kook {
	// linux使用到了openssl和crypto库-lssl -lcrypto
	// 初始化socket环境
	class init_base_socket {
	public:
		init_base_socket(bool ssl = true) {
			ctx = 0;
			// 加载资源，这里是写死的，自行修改
#ifdef _WIN32
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				exit(0);
			}
#endif
			if (ssl) {
				SSL_library_init();
				SSL_load_error_strings();
				ctx = SSL_CTX_new(SSLv23_client_method());
				if (0 == ctx) {
					exit(0);
				}
			}
		}

		~init_base_socket() {
			if (0 != ctx) {
				SSL_CTX_free(ctx);
				ctx = 0;
			}
#ifdef _WIN32
			WSACleanup();
#endif
		}
		// 获取当前ssl工作环境，如果结果为0，表示没有初始化ssl
		SSL_CTX *get_ctx() {
			return ctx;
		}

	protected:
		// ssl会话环境句柄
		SSL_CTX *ctx;
#ifdef _WIN32
		WSADATA wsaData;
#endif
	};


	// X509证书内容
	typedef struct _SSL_X509_S_ {
		string version;      // 证书版本(0代表V1；1代表V2；2代表V3，目前绝大多数证书都是V3版本)
		string serial;       // 证书的序列号，16进制
		string signature_algorithm; // 签名算法
		string oid;          // 签名算法oid
		string issuer;       // 证书颁发者
		string owner;        // 证书拥有者
		string validity_time; // 证书有效期，格式：2016-1-1 8:00:00/2016-12-30 8:00:00
		string pubkey;       // 公钥，16进制
		string dns;          // 证书上中的dns信息，用于指定证书可以在那些域名中使用，多个以,分隔
	}X509_cert_s;

	// X509有效时间结构
	typedef struct _ASN1_TIME_ {
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		_ASN1_TIME_() :year(0), month(0), day(0),
			hour(0), minute(0), second(0) {}

		string get_str_time() {
			char tempbuf[32] = "";
			STRFORMAT(tempbuf, sizeof(tempbuf), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
			return tempbuf;
		}
	}asn1_time;

	// 处理ssl证书
	class ssl_X509_cert {
	public:
		ssl_X509_cert(SSL *ssl_handle = 0) {
			this->ssl_handle = ssl_handle;
			this->is_cert = false;
		}
		// 重置ssl句柄和状态
		bool ssl_reset(SSL *ssl_handle = 0) {
			this->ssl_handle = ssl_handle;
			this->is_cert = false;
			return true;
		}
		// 判断是否有成功获取到证书
		bool is_cert_exist() const {
			return is_cert;
		}
		// 获取证书信息
		X509_cert_s get_X509_Cert();

	protected:
		// 解析ssl证书x509中的有效时间
		// 函数把ssl中的asn1时间转换了yyyy-mm-dd 00:00:00格式，asn是一个utc时间，与中国时间相差8小时
		void asn1_date_parse(const ASN1_TIME *time, int& year, int& month, int& day, int& hour, int& minute, int& second);

		bool is_cert;       // 是否已经成功加载了证书
		SSL *ssl_handle;    // ssl连接成功后的句柄
		X509_cert_s X509S;  // ssl证书x509标准，只提取基本的信息
	};

}

#endif