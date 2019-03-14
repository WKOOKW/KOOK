#include "radom_integer_iterator.h"

namespace kook {

	radom_integer_iterator::radom_integer_iterator(unsigned long n) {
		if (reset_n(n)) {
			return;
		}
		// 如果重置失败就初始化
		n = 0;
		rn = 0;
		re = 0;
		i = 1;
	}

	vector<unsigned long long> radom_integer_iterator::decompose_prime_factor(unsigned long long x) {
		vector<unsigned long long> prime_vector;
		unsigned long long i = 0;
		// 0和1不属于质数也不属于合数
		if (0 == x || 1 == x) {
			return prime_vector;
		}
		// 2和3本身就是质数(也不属于合数，最小的合数是4)，无法再分解
		if (2 == x || 3 == x) {
			prime_vector.push_back(x);
			return prime_vector;
		}
		// 分解质因数
		for (i = 2; x != 1; i++) {
			if (x%i == 0) {
				x /= i;
				prime_vector.push_back(i);
				i--;    //i--和i++使得i的值不变，即能把N含有的所有的当前质因数除尽
			}
		}
		return prime_vector;
	}

	bool radom_integer_iterator::is_prime_number(unsigned long long x) {
		// 2和3本身就是质数
		if (x <= 3) {
			return (x > 1);
		}
		// 质数大于等于2 不能被它本身和1以外的数整除，这里通过整除2和3来快速判断(因为x在这里不可能等于2和3)
		if (x % 2 == 0 || x % 3 == 0) {
			return false;
		}
		// 循环判断
		for (unsigned long long i = 5; i * i <= x; i += 6)
		{
			if (x % i == 0 || x % (i + 2) == 0) {
				return false;
			}
		}
		return true;
	}

	unsigned long long radom_integer_iterator::mul_mod(unsigned long long a, unsigned long long b, unsigned long long n) {
		return a * b % n;
	}

	unsigned long long radom_integer_iterator::pow_mod(unsigned long long base, unsigned long long pow, unsigned long long n) {
		unsigned long long a = base, b = pow, c = 1;
		while (b)
		{
			while (!(b & 1))
			{
				b >>= 1; // a = a*a%n;
				a = mul_mod(a, a, n);
			} b--; // c = a*c%n;
			c = mul_mod(a, c, n);
		}
		return c;
	}

	unsigned long long radom_integer_iterator::calculate_common_divisor(unsigned long long x, unsigned long long y) {
		unsigned long long temp;
		while (y != 0) {
			temp = y;
			y = x % y;
			x = temp;
		}
		return x;
	}

	int radom_integer_iterator::randint(int n) {
		n -= 1;
		if (n < 2) return 0;
		if (n == 2) return 2;
		// 注意：在过快的生成随机数时，会出现大量重复
		srand((unsigned int)time(0) + clock());
		return (rand() % (n - 2)) + 2;
	}


	rsapq radom_integer_iterator::findpq(vector<unsigned long long>& prime, unsigned long long x) {
		size_t psize = prime.size();
		rsapq pq;
		// 如果分解质数数量少于2就表示错误的n
		if (psize < 2) {
			return pq;
		}
		// 如果分解的质数数量等于2，并不相等，表示p和q成立
		if (psize == 2) {
			if (prime.at(0) != prime.at(1)) {
				pq.rsap = prime.at(0);
				pq.rsaq = prime.at(1);
			}
			return pq;
		}
		// 如果分解的质数数量大于2，就通过最笨的方法，遍历查找
		// 之前p只取分解的最后一个质数，一般是最大的质数，但如果质数过大，在后期会造成生成的n过大
		// (比如用户要生成1-100000000的随机序列，在分解质数后，新生成的n大于用户的范围，如果取最大质数，新生成的n会大于用户范围太多，会造成过多的空循环，影响效率)
		// 所以这里取第一个，最小的质数
		pq.rsap = prime.at(0);
		// 通过除法来得出其它分解质数之积
		pq.rsaq = x / pq.rsap;
		// 遍历查找质数
		while (!is_prime_number(++pq.rsaq)) {}
		return pq;
	}

	unsigned long long radom_integer_iterator::finde(rsapq &pq) {
		unsigned long long on = (pq.rsap - 1) * (pq.rsaq - 1);
		// e必须在1-on之间，并且e与on互质
		// 为了防止e过大，模幂效率过低，这里将e设置得尽量小，范围在1-65537
		// 随机生成e的目的是为了每次生成同样范围的随机序列时，序列数的顺序不同，达到最大随机效果
		int e = 0;
		if (on > 65537) {
			e = randint(65538);
		}
		else {
			e = randint((int)on);
		}
		// 如果on过小，随机数为0，此时e为1，这样将不会产生随机序列
		// 循环查找与on互质的e，这里不做e是否大于on的判断，因为最坏情况是e=on-1，因为两个连续的自然数一定是互质数
		while (calculate_common_divisor((unsigned long long)e, on) != 1)
		{
			e++;
		}
		return (unsigned long long)e;
	}

	bool radom_integer_iterator::init() {
		// 初始化随机算法参数，这里使用了一些低效的循环遍历，会比较慢，但他属于一次性运算
		rn = n;
		i = 1;
		// 如果n<=6就不进行随机序列
		if (n <= 6) {
			re = 0;
			rpq.rsap = 0;
			rpq.rsaq = 0;
			return true;
		}
		// 循环分解，直到找到合适的合数
		vector<unsigned long long> test;
		while (true)
		{
			test = decompose_prime_factor(rn);
			rpq = findpq(test, rn);
			if (rpq.rsap == 0 || rpq.rsaq == 0) {
				rn++;
				continue;
			}
			break;
		}
		// 重置rn
		rn = rpq.rsap * rpq.rsaq;
		// 计算e
		re = finde(rpq);
		return true;
	}

	bool radom_integer_iterator::reset_n(unsigned long n) {
		if (n == 0 || n > KOOK_MAX_RADOM_INTEGER_N) {
			return false;
		}
		this->n = (unsigned long long)n;
		return init();
	}

	bool radom_integer_iterator::reset_e() {
		if (n == 0 || rn == 0) {
			return false;
		}
		// 初始序列计数
		i = 1;
		// 不对<=6的范围生成随机序列，但返回true
		if (n <= 6) {
			re = 0;
			return true;
		}
		// 重新生成e
		re = finde(rpq);
		return true;
	}

	unsigned long radom_integer_iterator::next() {
		if (n == 0 || rn == 0) {
			return 0;
		}
		// 返回下一个随机序列数
		unsigned long long c = 0;
		// 当n==rn的时候，正常生成的随机序列范围是1-（n-1），因为当i=n的时候，c=0，所以这里直接判断
		if (n == rn) {
			if (i == rn) {
				i++;
				return (unsigned long)n;
			}
			else if (i > rn){
				i = 1;
				return 0;
			}
		}
		else {
			if (i >= rn) {
				i = 1;
				return 0;
			}
		}
		// 当n<6的时候不生成随机序列
		if (rn <= 6) {
			return (unsigned long)i++;
		}
		// 循环生成随机序列，这里使用循环，是因为rn大于n的时候，要过滤超出n的数
		while (true)
		{
			c = pow_mod(i++, re, rn);
			// 如果c==0表示i>=rn，此时表示序列生成完毕。因为如果n==rn，这里将不可能出现i>=rn，如果这里出现i>=rn，那么肯定rn>n
			if (c == 0) {
				i = 1;
				break;
			}
			if (c <= n) {
				break;
			}
		}
		return (unsigned long)c;
	}

}
