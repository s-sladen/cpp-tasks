#pragma once

#include <cstdlib>
#include <string>
#include <limits>
#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <stdexcept>

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
		: sign(bi.sign)
		, values(bi.values)
	{}

	bigint(bigint && bi)
		: sign(bi.sign)
		, values(std::move(bi.values))
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
				for (size_t k = i + j + 1; carry != 0; ++k) {
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
	static constexpr int RADIX = 1000000000;
	bool sign;
	std::vector<int> values;
};
