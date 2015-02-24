#pragma once

#include <string>
#include <vector>
#include <iostream>

class bigint {
public:
	bigint();
	bigint(int value);
	bigint(std::string const& value);
	bigint(bigint const& bi);
	bigint(bigint && bi);

	bigint& operator=(bigint bi);

	bigint operator-() const;

	bigint& operator+=(bigint const& bi);
	bigint& operator-=(bigint const& bi);
	bigint& operator*=(bigint const& bi);

	bigint operator+(bigint const& bi) const;
	bigint operator-(bigint const& bi) const;
	bigint operator*(bigint const& bi) const;

	bool operator<(bigint const& bi) const;
	bool operator>(bigint const& bi) const;
	bool operator<=(bigint const& bi) const;
	bool operator>=(bigint const& bi) const;

	bool operator==(bigint const& bi) const;
	bool operator!=(bigint const& bi) const;

	operator std::string() const;

	friend std::ostream& operator<<(std::ostream& os, bigint const& bi);
	friend std::istream& operator>>(std::istream& is, bigint & bi);

private:
	bigint(bool sign, std::vector<int> values);

	static constexpr int DIGITS = 9;
	static constexpr int RADIX = 1000000000;
	bool sign;
	std::vector<int> values;
};
