#pragma once

#include <string>
#include <vector>
#include <iostream>

class bigint {
public:
    /**
    * Constructs bigint with 0 value
    */
	bigint();
    /**
    * Constructs bigint with given value
    * @param value of new bigint
    */
	bigint(int value);
    /**
    * Constructs bigint with given value
    * @param value string representation of value
    * @throws std::runtime_error if string is not valid number representation
    */
	bigint(std::string const& value);
    /**
    * Copies existing bigint
    * @param bi value to copy
    */
	bigint(bigint const& bi);
    /**
    * Move constructor
    * @param bi value to move
    */
	bigint(bigint && bi);

    /**
    * Assigns existing bigint
    * @param bi value to assign
    * @return this
    */
	bigint& operator=(bigint bi);

    /**
    * Unary negation operator
    * @return bigint equals -this
    */
	bigint operator-() const;

    /**
    * Adds given bigint to this.
    * Result is equivalent to this = this + bi.
    * @param bi value to add
    * @return this
    */
	bigint& operator+=(bigint const& bi);
    /**
    * Subtracts given bigint from this.
    * Result is equivalent to this = this - bi.
    * @param bi value to subtract
    * @return this
    */
	bigint& operator-=(bigint const& bi);
    /**
    * Multiplies this bigint by given value.
    * Result is equivalent to this = this * bi.
    * @param bi value to multiply
    * @return this
    */
	bigint& operator*=(bigint const& bi);

    /**
    * Returns a bigint whose value is (this + bi)
    * @param bi value to be added to this bigint
    * @return this - bi
    */
	bigint operator+(bigint const& bi) const;
    /**
    * Returns a bigint whose value is (this - bi)
    * @param bi value to be subtracted from this bigint
    * @return this - bi
    */
	bigint operator-(bigint const& bi) const;
    /**
    * Returns a bigint whose value is (this * bi)
    * @param bi value to be multiplied by this bigint
    * @return this * bi
    */
	bigint operator*(bigint const& bi) const;

    /**
    * Checks if this bigint is less than given.
    * @param bi value to compare with
    * @return true if this < bi, false otherwise
    */
	bool operator<(bigint const& bi) const;
    /**
    * Checks if this bigint is greater than given.
    * @param bi value to compare with
    * @return true if this > bi, false otherwise
    */
	bool operator>(bigint const& bi) const;
    /**
    * Checks if this bigint is less than or equal to given.
    * @param bi value to compare with
    * @return true if this <= bi, false otherwise
    */
	bool operator<=(bigint const& bi) const;
    /**
    * Checks if this bigint is greater than or equal to given.
    * @param bi value to compare with
    * @return true if this >= bi, false otherwise
    */
	bool operator>=(bigint const& bi) const;

    /**
    * Checks if this bigint is equal to given.
    * @param bi value to compare with
    * @return true if this == bi, false otherwise
    */
	bool operator==(bigint const& bi) const;
    /**
    * Checks if this bigint is not equal to given.
    * @param bi value to compare with
    * @return true if this != bi, false otherwise
    */
	bool operator!=(bigint const& bi) const;

    /**
    * Returns decimal string representation of this bigint. Guaratied that bigint((std::string) this) == this.
    * @return decimal string representation of this bigint.
    */
	operator std::string() const;

    /**
    * Prints decimal string representation of given bigint to the given std::ostream.
    * Result is equivalent to os << (std::string) bi;
    * @returns given std::ostream
    */
	friend std::ostream& operator<<(std::ostream& os, bigint const& bi);
    /**
    * Reads bigint from given std::istream to the given bigint variable.
    * @returns given std::istream
    * @throws std::runtime_error if this stream does not contain valid number.
    */
	friend std::istream& operator>>(std::istream& is, bigint & bi);

private:
	bigint(bool sign, std::vector<int> values);

	static constexpr int DIGITS = 9;
	static constexpr int RADIX = 1000000000;
	bool sign;
	std::vector<int> values;
};
