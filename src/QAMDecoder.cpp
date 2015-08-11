/*
 * QAMDecoder.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: fede
 */

#include "QAMDecoder.h"

QAMDecoder::QAMDecoder(int k, int n, boost::numeric::ublas::matrix<int> matrix,
		int symbols, int passes) {
	this->k = k;
	this->n = n;
	this->passes = passes;
	this->symbols = symbols;
	this->matrix = matrix;
	p = n - k;

	//Symbol graph
	s = n / log2(symbols);

	initializeGraph();
}

bool QAMDecoder::parityCheck() {
	for (int i = 0; i < p; i++) {
		int sum = 0;
		for (int j = 0; j < n; j++) {
			sum += matrix(i, j)
					* encoded.at(j)->llr2val(encoded.at(j)->getValue());
		}
		if (sum % 2 != 0) {
			return false;
		}
	}
	return true;
}

std::vector<int> QAMDecoder::decode(std::vector<double> phase,
		std::vector<double> quad, double n0) {
	if (phase.size() != quad.size() || phase.size() != s) {
		std::stringstream str;
		str << "Invalid input size";
		throw std::invalid_argument(str.str());
	}

	//LLR symbol node initialization
	for (int i = 0; i < s; i++) {
		//TODO fix initialization
		received.at(i)->setValue(phase.at(i), quad.at(i));
		if (DEBUG) {
			std::cout << phase.at(i) << " " << quad.at(i) << " ";
		}
	}
	if (DEBUG) {
		std::cout << " received\n";
	}

	//Message passing (stops when Hc=0 or after a max number of cycles)
	int pass = 0;
	bool check = parityCheck();
	while (!check && pass < passes) {
		forwardPassing();
		if (DEBUG) {
			for (int i = 0; i < k; i++) {
				std::cout << parity.at(i)->getValue() << " ";
			}
			std::cout << "LLR (check)\n";
		}
		check = parityCheck();
		if (check) {
			break;
		}
		backwardPassing();
		if (DEBUG) {
			for (int i = 0; i < n; i++) {
				std::cout << encoded.at(i)->getValue() << " ";
			}
			std::cout << "LLR (var)\n";
		}
		check = parityCheck();
		pass += 2;
	}

	//Decoded vector
	std::vector<int> decoded;
	for (int i = 0; i < n; i++) {
		double in = encoded.at(i)->getValue();
		decoded.push_back(encoded.at(i)->llr2val(in));
	}
	resetGraph();

	return decoded;
}

void QAMDecoder::forwardPassing() {
	for (int i = 0; i < s; i++) {
		received.at(i)->sendMessages();
	}
	for (int i = 0; i < n; i++) {
		encoded.at(i)->sendMessages();
	}
}

void QAMDecoder::backwardPassing() {
	for (int i = 0; i < p; i++) {
		parity.at(i)->sendMessages();
	}
	for (int i = 0; i < s; i++) {
		received.at(i)->updateValue();
	}
}

void QAMDecoder::initializeGraph() {
	//Node initialization
	for (int i = 0; i < s; i++) {
		//Node type BP_SYMBOL
		received.push_back(new SymbolNode(0, 0, log2(symbols)));
	}
	for (int i = 0; i < n; i++) {
		encoded.push_back(new BpNode(0, BpNode::BP_VAR));
	}
	for (int i = 0; i < p; i++) {
		parity.push_back(new BpNode(0, BpNode::BP_CHECK));
	}

	//Right-to-center neighbors
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < n; j++) {
			if (matrix(i, j) != 0) {
				parity.at(i)->addNeighbor(encoded.at(j));
			}
		}
	}

	//Center-to-right neighbors
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < p; i++) {
			if (matrix(i, j) != 0) {
				encoded.at(j)->addNeighbor(parity.at(i));
			}
		}
	}

	//TODO interleaver!
	int modSize = log2(symbols);
	//Left-to-center neighbors
	for (int i = 0; i < s; i++) {
		for (int j = 0; j < modSize; j++) {
			int b = i * modSize + j;
			received.at(i)->addNeighbor(encoded.at(b));
		}
	}
}

void QAMDecoder::resetGraph() {
	//Reset left side nodes
	for (int i = 0; i < n; i++) {
		received.at(i)->resetNode();
	}

	//Reset center nodes
	for (int i = 0; i < n; i++) {
		encoded.at(i)->resetNode();
	}

	//Reset right side nodes
	for (int i = 0; i < p; i++) {
		parity.at(i)->resetNode();
	}
}

QAMDecoder::~QAMDecoder() {
	received.clear();
	encoded.clear();
	parity.clear();
}

