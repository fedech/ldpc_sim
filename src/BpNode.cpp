#include "BpNode.h"
#include <cmath>
#include<iostream>

BpNode::BpNode(double initialValue, BpType type) {
	this->type = type;
	this->value = initialValue;
}

void BpNode::addNeighbor(BpNode* node) {
	neighbor n;
	n.node = node;
	n.message = 0;
	neighbors.push_back(n);
}

void BpNode::sendMessages() {
	for (int i = 0; i < neighbors.size(); i++) {
		double llr = findMessage(neighbors.at(i));
		message(neighbors.at(i).node, llr);
	}
}

double BpNode::getValue() {
	return value;
}

void BpNode::setValue(double newValue) {
	if (std::abs(newValue) < MAX_LLR) {
		value = newValue;
	} else {
		value = MAX_LLR * sign(newValue);
	}
}

void BpNode::resetNode() {
	value = 0;
	for (int i = 0; i < neighbors.size(); i++) {
		neighbors.at(i).message = 0;
	}
}

void BpNode::message(BpNode *rec, double message) {
	for (int i = 0; i < rec->neighbors.size(); i++) {
		if (this == rec->neighbors.at(i).node) {
			rec->neighbors.at(i).message = message;
		}
	}
	if (rec->type == BP_VAR) {
		double val = rec->getValue() + message;
		rec->setValue(val);
	}
}

double BpNode::findMessage(neighbor rec) {
	double message = 0;
	if (type == BP_VAR) {
		message = value;
		for (int i = 0; i < neighbors.size(); i++) {
			if (rec.node != neighbors.at(i).node) {
				double llr = neighbors.at(i).message;
				message += llr;
			}
		}
	} else if (type == BP_CHECK) {
		message = 1;
		double min = MAX_LLR;
		for (int i = 0; i < neighbors.size(); i++) {
			if (rec.node != neighbors.at(i).node) {
				double llr = neighbors.at(i).message;
				if (MINSUM) {
					if (std::abs(llr) < min) {
						min = std::abs(neighbors.at(i).message);
					}
					message *= sign(llr);
				} else {
					if (std::abs(llr) < MAX_LLR) {
						message *= tanh(llr / 2);
					} else {
						message *= sign(llr);
					}
				}
			}
		}
		if (MINSUM) {
			message *= min;
		} else {
			if (std::abs(message) < 1) {
				message = 2 * atanh(message);
			} else {
				message = sign(message) * MAX_LLR;
			}
		}
	}
	if (std::abs(message) > MAX_LLR) {
		message = sign(message) * MAX_LLR;
	}
	return message;
}

BpNode::~BpNode() {
	//No pointers to dereference
}
