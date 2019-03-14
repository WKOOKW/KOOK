#include "X509_cert.h"


namespace kook {

	void ssl_X509_cert::asn1_date_parse(const ASN1_TIME *time, int& year, int& month, int& day, int& hour, int& minute, int& second)
	{
		// 函数把ssl中的asn1时间转换了yyyy-mm-dd 00:00:00格式，asn是一个utc时间，与中国时间相差8小时
		const char* str = (const char*)time->data;
		size_t i = 0;
		// 获取UTC年份
		if (time->type == V_ASN1_UTCTIME) {
			year = (str[i++] - '0') * 10;
			year += (str[i++] - '0');
			year += (year < 70 ? 2000 : 1900);
		}// 获取其它类型的年份
		else if (time->type == V_ASN1_GENERALIZEDTIME) {
			year = (str[i++] - '0') * 1000;
			year += (str[i++] - '0') * 100;
			year += (str[i++] - '0') * 10;
			year += (str[i++] - '0');
		}
		month = (str[i++] - '0') * 10;
		month += (str[i++] - '0');
		day = (str[i++] - '0') * 10;
		day += (str[i++] - '0');
		hour = (str[i++] - '0') * 10;
		hour += (str[i++] - '0');
		minute = (str[i++] - '0') * 10;
		minute += (str[i++] - '0');
		second = (str[i++] - '0') * 10;
		second += (str[i++] - '0');
		return;
	}

	X509_cert_s ssl_X509_cert::get_X509_Cert() {
		if (0 == ssl_handle) {
			return X509_cert_s();
		}
		if (is_cert) return X509S;
		// 获取ssl证书信息
		X509* sCert = SSL_get_peer_certificate(ssl_handle);
		if (0 == sCert) {
			return X509_cert_s();
		}
		HEX hex;
		hex.set_hex_mode(kook::TEXT_TO_HEX_ADD_BLANK, false);
		char oidBuf[128] = "";
		X509_NAME *issuer;
		X509_NAME *owner;
		ASN1_BIT_STRING *pubKeyStr;
		asn1_time aTime;
		int numerical = 0, num = 0;
		///////////////////
		X509S.dns = "";
		// 获取证书中的dnSName信息，多个以,号分隔
		GENERAL_NAMES *subjectAltNames = (GENERAL_NAMES*)X509_get_ext_d2i(sCert, NID_subject_alt_name, NULL, NULL);
		if (0 != subjectAltNames) {
			// 获取证书的dns信息
			num = sk_GENERAL_NAME_num(subjectAltNames);
			for (numerical = 0; numerical < num; numerical++) {
				GENERAL_NAME* generalName = sk_GENERAL_NAME_value(subjectAltNames, numerical);
				// 当type==GEN_DNS表示要获取的是dns
				if (0 == generalName || generalName->type != GEN_DNS) {
					continue;
				}
				X509S.dns.append((const char *)generalName->d.dNSName->data, generalName->d.dNSName->length);
				if (numerical != num - 1) {
					X509S.dns.push_back(',');
				}
			}
		}
		////////////////////
		// 获取版本信息,0代表V1,1代表V2,2代表V3
		numerical = X509_get_version(sCert) + 49;
		X509S.version = "V";
		X509S.version += (char)numerical;
		// 获取序列号
		hex.hex_input((const char *)sCert->cert_info->serialNumber->data, sCert->cert_info->serialNumber->length);
		X509S.serial = hex.get_to_hex();
		// 获取签名算法
		numerical = OBJ_obj2nid(sCert->sig_alg->algorithm);
		X509S.signature_algorithm = OBJ_nid2ln(numerical);
		// 获取签名算法oid
		numerical = OBJ_obj2txt(oidBuf, sizeof(oidBuf), sCert->cert_info->signature->algorithm, 1);
		X509S.oid = oidBuf;
		// 获取证书颁发者
		issuer = X509_get_issuer_name(sCert);
		X509S.issuer = X509_NAME_oneline(issuer, 0, 0);
		// 获取证书拥有者
		owner = X509_get_subject_name(sCert);
		X509S.owner = X509_NAME_oneline(owner, 0, 0);
		// 获取有效时间
		X509S.validity_time = "UTC/";
		asn1_date_parse(X509_get_notBefore(sCert), aTime.year, aTime.month, aTime.day, aTime.hour, aTime.minute, aTime.second);
		X509S.validity_time += aTime.get_str_time();
		X509S.validity_time += "/";
		asn1_date_parse(X509_get_notAfter(sCert), aTime.year, aTime.month, aTime.day, aTime.hour, aTime.minute, aTime.second);
		X509S.validity_time += aTime.get_str_time();
		// 获取公钥
		pubKeyStr = X509_get0_pubkey_bitstr(sCert);
		hex.hex_input((const char *)pubKeyStr->data, pubKeyStr->length);
		X509S.pubkey = hex.get_to_hex();
		// 释放证书
		X509_free(sCert);
		this->is_cert = true;
		return X509S;
	}

}
