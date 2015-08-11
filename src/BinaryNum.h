/*
 * BinaryNum.h
 *
 *  Created on: Nov 29, 2014
 *      Author: fede
 */

#ifndef BINARYNUM_H_
#define BINARYNUM_H_

class BinaryNum {
public:
	BinaryNum();
	virtual ~BinaryNum();

	BinaryNum(int);
	int getValue();
	void setValue(int val);

	bool operator==(const BinaryNum& other) const {
		return value == other.value;
	}
	bool operator==(const int& other) const {
		return value == other;
	}
	bool operator!=(const BinaryNum& other) const {
		return value != other.value;
	}
	bool operator!=(const int& other) const {
		return value != other;
	}
	bool operator>(const BinaryNum& other) const {
		return value > other.value;
	}
	bool operator<(const BinaryNum& other) const {
		return value < other.value;
	}
	BinaryNum operator+(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value + other.value) % 2;
		return result;
	}
	BinaryNum operator-(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value + other.value) % 2;
		return result;
	}
	BinaryNum operator*(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value * other.value);
		return result;
	}
	BinaryNum operator/(const BinaryNum& other) const {
		BinaryNum result;
		result.value = 0;
		if (value != other.value) {
			result.value = 1;
		}
		return result;
	}
	BinaryNum operator+=(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value + other.value) % 2;
		return result;
	}
	BinaryNum operator-=(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value + other.value) % 2;
		return result;
	}
	BinaryNum operator/=(const BinaryNum& other) const {
		BinaryNum result;
		result.value = 0;
		if (value != other.value) {
			result.value = 1;
		}
		return result;
	}
	BinaryNum operator*=(const BinaryNum& other) const {
		BinaryNum result;
		result.value = (value * other.value);
		return result;
	}
protected:
	int value;
};

#endif /* BINARYNUM_H_ */
