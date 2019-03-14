#ifndef KOOK_RADOM_INTEGER_ITERATOR_H_H
#define KOOK_RADOM_INTEGER_ITERATOR_H_H

/*
伪随机序列数迭代生成器，基于RSA算法
实现：不懂
*/

// 这是一个伪随机序列数迭代生成器，用于解决快速迭代1-n范围内的随机序列（这个n最好在1亿内，否则需要做大数运算），并且保证每次生成的随机数不重复，在迭代n次后，保证序列能覆盖1-n的所有数
// 此随机序列迭代生成算法使用的是rsa加密算法(加密的特性使他天生具有生成随机数的能力)，优点是速度快，内存占用极低
// 此随机数适用场景多，比如用于随机扫描一个B段IP端口时，此随机迭代器就能非常完美的解决
// 由于没有使用大数库，请注意n的范围，防止漏出

// 只需要利用rsa的加密部分就可以实现，所以这只里需要计算出p,q,e和新的n就可以，不需要再计算d来解密

#include <vector>
using std::vector;

#include <stdlib.h>
#include <time.h>

// 范围限制宏，即限制用户输入的n，最大不可以超过宏指定的数
#define KOOK_MAX_RADOM_INTEGER_N 100000000

namespace kook {
	// 保存pq值
	typedef struct _RSAPQ {
		unsigned long long rsap;
		unsigned long long rsaq;
		_RSAPQ() :rsap(0), rsaq(0) {};
	}rsapq;

	// 随机序列迭代对象
	// 当n<=6的时候，将不能产生出随机序列，因为无法生成有效的e
	class radom_integer_iterator {
	public:
		radom_integer_iterator() : n(0), rn(0), re(0), i(1) {}
		// 通过unsigned long来限制用户输入过大
		radom_integer_iterator(unsigned long n);
		// 分解质因数，把一个合数分解成若干个质因数的乘积的形式，即求质因数的过程叫做分解质因数(分解质因数只针对合数)
		// 合数指自然数中除了能被1和本身整除外，还能被其他数（0除外）整除的数
		static vector<unsigned long long> decompose_prime_factor(unsigned long long x);
		// 判断一个数是否是质数
		static bool is_prime_number(unsigned long long x);
		// 计算 a * b % n
		static unsigned long long mul_mod(unsigned long long a, unsigned long long b, unsigned long long n);
		// 模幂运算是RSA的核心算法，直接决定RSA算法的性能，通常都是先将幂模运算转化为乘模运算
		// 返回值 x = base^pow mod n
		static unsigned long long pow_mod(unsigned long long base, unsigned long long pow, unsigned long long n);
		// 辗转相除法来得出两整数的最大公约数
		// 1. x%y得余数c
		// 2. 若c=0，则y即为两数的最大公约数
		// 3. 若c≠0，则x=y，y=c，再回去执行1
		static unsigned long long calculate_common_divisor(unsigned long long x, unsigned long long y);
		// 产生一个大于2小于n范围内的随机数，因为1<e>n
		static int randint(int n);
		// 重置n
		bool reset_n(unsigned long n);
		// 重置e，当范围n不变，但想改变随机序列的时候，就可以通过重置e来实现。注意：重置e，那么迭代i也将初始化
		bool reset_e();
		// 迭代随机序列数
		// 如果返回结果为0，表示迭代结束，如果再继续迭代，将重复上一次的随机序列
		unsigned long next();
	private:
		// 初始化随机参数，如n,p,q,e
		bool init();
		// 计算pq
		rsapq findpq(vector<unsigned long long>& prime, unsigned long long x);
		// 通过qp来得出e
		unsigned long long finde(rsapq &pq);
		// 用户指定的随机序列范围n
		unsigned long long n;
		// 新计算，用于随机算法的n，加密(c=i^re%rn)
		unsigned long long rn;
		// 用于随机算法的e
		unsigned long long re;
		// 保存pq值
		rsapq rpq;
		// 用于随机序列迭代的i，初始值为1
		unsigned long long i;
	};
}


#endif