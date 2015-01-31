#include <cstdlib>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <stdexcept>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE test_bigint
#include <boost/test/unit_test.hpp>

class bigint {
public:
	bigint()
		: bigint(0)
	{}

	bigint(int value) {
		sign = value < 0;
		if (value > RADIX || value < -RADIX) {
			values.push_back(abs(value % RADIX));
			values.push_back(abs(value / RADIX));
		} else {
			values.push_back(abs(value));
		}
	}

	bigint(std::string const& value) {
		if (value.length() == 0)
			throw std::runtime_error("empty string");
		sign = value[0] == '-';
		size_t len = value.length() - (sign ? 1 : 0);
		values.assign(len / DIGITS + (len % DIGITS != 0 ? 1 : 0), 0);
		int pw = 1;
		for (size_t i = 1; i <= len; ++i) {
			size_t idx = (i - 1) / DIGITS;
			if ((i - 1) % DIGITS == 0) {
				pw = 1;
			}
			char digit = value[value.length() - i];
			if (digit < '0' || digit > '9') {
				throw std::runtime_error("non digit");
			}
			values[idx] += (digit - '0') * pw;
			pw *= 10;
		}
		while (!values.empty() && values.back() == 0)
			values.pop_back();

		if (values.empty()) {
			values.push_back(0);
			sign = false;
		}
	}

	bigint(bigint const& bi)
		: values(bi.values)
		, sign(bi.sign)
	{}

	bigint(bigint && bi)
		: values(std::move(bi.values))
		, sign(bi.sign)
	{}

	bigint& operator=(bigint bi) {
		std::swap(values, bi.values);
		std::swap(sign, bi.sign);
		return *this;
	}

	bigint operator-() const {
		if (values.size() > 1 || values[0])
			return bigint(!sign, values);
		else
			return *this;
	}

	bigint& operator+=(bigint const& bi) {
		if (sign != bi.sign)
			return *this -= -bi;
		size_t len = std::max(values.size(), bi.values.size());
		values.resize(len);
		int carry = 0;
		for (size_t i = 0; i < len; ++i) {
			values[i] += (i < bi.values.size() ? bi.values[i] : 0) + carry;
			carry = values[i] / RADIX;
			values[i] %= RADIX;
		}
		if (carry)
			values.push_back(carry);
		return *this;
	}

	bigint& operator-=(bigint const& bi) {
		if (sign != bi.sign)
			return *this += -bi;
		if (*this == bi)
			return *this = 0;
		bool less = (*this < bi) ^ sign;
		sign ^= less;
		size_t len = std::max(values.size(), bi.values.size());
		values.resize(len);
		int carry = 0;
		for (size_t i = 0; i < len; ++i) {
			values[i] = (less ? -1 : 1) * (values[i] - (i < bi.values.size() ? bi.values[i] : 0)) - carry;
			if (values[i] < 0) {
				carry = 1;
				values[i] += RADIX;
			} else {
				carry = 0;
			}
		}
		while (values.back() == 0)
			values.pop_back();
		return *this;
	}

	bigint& operator*=(bigint const& bi) {
		return *this = *this * bi;
	}

	bigint operator+(bigint const& bi) const {
		bigint res = *this;
		res += bi;
		return res;
	}

	bigint operator-(bigint const& bi) const {
		bigint res = *this;
		res -= bi;
		return res;
	}

	bigint operator*(bigint const& bi) const {
		std::vector<int> result(values.size() + bi.values.size() - 1);
		for (size_t i = 0; i < values.size(); ++i)
			for (size_t j = 0; j < bi.values.size(); ++j) {
				long long prod = (long long) values[i] * bi.values[j];
				result[i + j] += prod % RADIX;
				int carry = result[i + j] / RADIX + prod / RADIX;
				result[i + j] %= RADIX;
				for (int k = i + j + 1; carry != 0; ++k) {
					if (k >= result.size()) {
						result.push_back(carry);
						carry = 0;
					} else {
						result[k] += carry;
						carry = result[k] / RADIX;
						result[k] %= RADIX;
					}
				}
			}
		return bigint(sign ^ bi.sign, std::move(result));
	}

	bool operator<(bigint const& bi) const {
		if (sign != bi.sign)
			return sign;
		if (values.size() != bi.values.size())
			return (values.size() < bi.values.size()) ^ sign;
		for (size_t i = 1, len = values.size(); i <= len; ++i)
			if (values[len - i] != bi.values[len - i])
				return (values[len - i] < bi.values[len - i]) ^ sign;
		return false;
	}

	bool operator>(bigint const& bi) const {
		return bi < *this;
	}

	bool operator<=(bigint const& bi) const {
		return !(bi < *this);
	}

	bool operator>=(bigint const& bi) const {
		return !(*this < bi);
	}

	bool operator==(bigint const& bi) const {
		return sign == bi.sign && values == bi.values;
	}

	bool operator!=(bigint const& bi) const {
		return !(*this == bi);
	}

	operator std::string() const {
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

	friend std::ostream& operator<<(std::ostream& os, bigint const& bi) {
		if (bi.sign) {
			os << '-';
		}
		os << bi.values.back();
		os << std::setfill('0');
		for (size_t i = 2, len = bi.values.size(); i <= len; ++i) {
			os << std::setw(DIGITS) << bi.values[len - i];
		}
		return os;
	}

	friend std::istream& operator>>(std::istream& is, bigint & bi) {
		std::string str;
		is >> str;
		bi = str;
		return is;
	}

private:
	bigint(bool sign, std::vector<int> values)
		: sign(sign)
		, values(std::move(values))
	{}

	static constexpr int DIGITS = 9;
	static constexpr int RADIX = 1e9;
	bool sign;
	std::vector<int> values;
};

void check_int(int value) {
	std::stringstream ss;
	ss << value;
	BOOST_CHECK_EQUAL((std::string) bigint(value), ss.str());
}

void check_string(std::string const& value) {
	BOOST_CHECK_EQUAL((std::string) bigint(value), value);
}

BOOST_AUTO_TEST_CASE(bigint_construct)
{
	check_int(0);
	check_int(100);
	check_int(-100);
	check_int(2147483647);
	check_int(-2147483648);

	check_string("0");
	check_string("100");
	check_string("-100");
	check_string("2147483647");
	check_string("-2147483648");
	check_string("2347012498126481624781624781263512456127341782647162546918273");
	check_string("-2345781234891379048623457237848123741238974193849012378904819248");

	BOOST_CHECK_EQUAL((std::string) bigint("-0"), "0");
	BOOST_CHECK_EQUAL((std::string) bigint("000000000000000000000000100"), "100");
	BOOST_CHECK_EQUAL((std::string) bigint("-000000000000000000000000100"), "-100");
}

void check_add(std::string const& a, std::string const& b, std::string const& res) {
	BOOST_CHECK_EQUAL(bigint(a) + bigint(b), bigint(res));
	BOOST_CHECK_EQUAL(bigint(b) + bigint(a), bigint(res));
}

BOOST_AUTO_TEST_CASE(bigint_add)
{
	check_add("0", "-0", "0");
	check_add("100", "-100", "0");
	check_add("-100", "100", "0");
	check_add("100", "100", "200");
	check_add("-100", "-100", "-200");

	check_add("2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "-2343434222393252566998675613066860228782846852066365216357900975");
	check_add("-2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "2343434222393252566998675613066860228782846852066365216357900975");
	check_add("2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "2348128247389505530248238862629387253695101535631659541451737521");
	check_add("-2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "-2348128247389505530248238862629387253695101535631659541451737521");
}

void check_sub(std::string const& a, std::string const& b, std::string const& res) {
	BOOST_CHECK_EQUAL(bigint(a) - bigint(b), bigint(res));
	BOOST_CHECK_EQUAL(bigint(b) - bigint(a), -bigint(res));
}

BOOST_AUTO_TEST_CASE(bigint_subtract)
{
	check_sub("0", "-0", "0");
	check_sub("100", "100", "0");
	check_sub("-100", "-100", "0");
	check_sub("100", "-100", "200");
	check_sub("-100", "100", "-200");

	check_sub("2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "-2343434222393252566998675613066860228782846852066365216357900975");
	check_sub("-2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "2343434222393252566998675613066860228782846852066365216357900975");
	check_sub("2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "2348128247389505530248238862629387253695101535631659541451737521");
	check_sub("-2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "-2348128247389505530248238862629387253695101535631659541451737521");
}

void check_mul(std::string const& a, std::string const& b, std::string const& res) {
	BOOST_CHECK_EQUAL(bigint(a) * bigint(b), bigint(res));
	BOOST_CHECK_EQUAL(bigint(b) * bigint(a), bigint(res));
}


BOOST_AUTO_TEST_CASE(bigint_multiply)
{
	check_mul("0", "-0", "0");
	check_mul("100", "-100", "-10000");
	check_mul("-100", "100", "-10000");
	check_mul("100", "100", "10000");
	check_mul("-100", "-100", "10000");

	check_mul("2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_mul("-2347012498126481624781624781263512456127341782647162546918273", "2345781234891379048623457237848123741238974193849012378904819248", "-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_mul("2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_mul("-2347012498126481624781624781263512456127341782647162546918273", "-2345781234891379048623457237848123741238974193849012378904819248", "5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
}

void check_less(std::string const& a, std::string const& b) {
	BOOST_CHECK(bigint(a) < bigint(b));
	BOOST_CHECK(!(bigint(b) < bigint(a)));

	BOOST_CHECK(bigint(b) > bigint(a));
	BOOST_CHECK(!(bigint(a) > bigint(b)));

	BOOST_CHECK(bigint(a) <= bigint(b));
	BOOST_CHECK(!(bigint(b) <= bigint(a)));

	BOOST_CHECK(bigint(b) >= bigint(a));
	BOOST_CHECK(!(bigint(a) >= bigint(b)));

	BOOST_CHECK(!(bigint(b) == bigint(a)));
	BOOST_CHECK(!(bigint(a) == bigint(b)));

	BOOST_CHECK(bigint(a) != bigint(b));
	BOOST_CHECK(bigint(b) != bigint(a));
}

void check_eq(std::string const& a, std::string const& b) {
	BOOST_CHECK(!(bigint(a) < bigint(b)));
	BOOST_CHECK(!(bigint(b) < bigint(a)));

	BOOST_CHECK(!(bigint(b) > bigint(a)));
	BOOST_CHECK(!(bigint(a) > bigint(b)));

	BOOST_CHECK(bigint(a) <= bigint(b));
	BOOST_CHECK(bigint(b) <= bigint(a));

	BOOST_CHECK(bigint(b) >= bigint(a));
	BOOST_CHECK(bigint(a) >= bigint(b));

	BOOST_CHECK(bigint(b) == bigint(a));
	BOOST_CHECK(bigint(a) == bigint(b));

	BOOST_CHECK(!(bigint(a) != bigint(b)));
	BOOST_CHECK(!(bigint(b) != bigint(a)));
}

void check_eq(std::string const& a) {
	check_eq(a, a);
}


BOOST_AUTO_TEST_CASE(bigint_compare)
{
	check_eq("0", "-0");
	check_eq("100", "0000000000000000000000000100");

	check_eq("2347012498126481624781624781263512456127341782647162546918273");
	check_eq("-2347012498126481624781624781263512456127341782647162546918273");
	check_eq("5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_eq("-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");

	check_less("-100", "0");
	check_less("0", "100");
	check_less("-100", "100");
	check_less("100", "200");
	check_less("-200", "-100");

	check_less("2347012498126481624781624781263512456127341782647162546918273", "3347012498126481624781624781263512456127341782647162546918273");
	check_less("-2347012498126481624781624781263512456127341782647162546918273", "3347012498126481624781624781263512456127341782647162546918273");
	check_less("-3347012498126481624781624781263512456127341782647162546918273", "2347012498126481624781624781263512456127341782647162546918273");
	check_less("-3347012498126481624781624781263512456127341782647162546918273", "-2347012498126481624781624781263512456127341782647162546918273");

	check_less("2347012498126481624781624781263512456127341782647162546918273", "5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_less("-2347012498126481624781624781263512456127341782647162546918273", "5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_less("-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704", "2347012498126481624781624781263512456127341782647162546918273");
	check_less("-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704", "-2347012498126481624781624781263512456127341782647162546918273");
}

void check_stream(std::string const& a) {
	bigint x = a;
	std::stringstream ss;
	ss << x;
	BOOST_CHECK_EQUAL(a, ss.str());
	bigint y;
	ss >> y;
	BOOST_CHECK_EQUAL(x, y);
}

BOOST_AUTO_TEST_CASE(bigint_stream_operators)
{
	check_stream("0");
	check_stream("-100");
	check_stream("100");
	check_stream("2347012498126481624781624781263512456127341782647162546918273");
	check_stream("-2347012498126481624781624781263512456127341782647162546918273");
	check_stream("5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
	check_stream("-5505577876160638521545911663481601164887929659284913815816157501078626421433445593272712739504712920054093596419550493318704");
}
