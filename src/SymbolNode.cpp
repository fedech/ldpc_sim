/*
 * SymbolNode.cpp
 *
 *  Created on: Jan 19, 2015
 *      Author: fede
 */

#include "SymbolNode.h"

SymbolNode::SymbolNode(double phase, double quad, int modSize) {
	this->phase = phase;
	this->quad = quad;
	this->modSize = modSize;
}

void SymbolNode::addNeighbor(BpNode* node) {
	neighbor n;
	n.node = node;
	n.message = 0;
	neighbors.push_back(n);
}

void SymbolNode::sendMessages() {
	for (int i = 0; i < neighbors.size(); i++) {
		//TODO calculate LLR
		double llr = 0;
		neighbors.at(i).node->setValue(llr);
	}
}

void SymbolNode::setValue(double phase, double quad) {
	this->phase = phase;
	this->quad = quad;
}

void SymbolNode::updateValue() {
	//TODO backwards??
}

void SymbolNode::resetNode() {
	phase = 0;
	quad = 0;
	for (int i = 0; i < neighbors.size(); i++) {
		neighbors.at(i).message = 0;
	}
}

SymbolNode::~SymbolNode() {
	//No pointers to dereference
}

