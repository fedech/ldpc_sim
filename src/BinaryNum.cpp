/*
 * BinaryNum.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: fede
 */

#include "BinaryNum.h"

BinaryNum::BinaryNum() {
	value = 0;
}

BinaryNum::BinaryNum(int val) {
	value = val;
}

int BinaryNum::getValue() {
	return value;
}

void BinaryNum::setValue(int val) {
	value = val;
}

BinaryNum::~BinaryNum() {
	// TODO Auto-generated destructor stub
}

