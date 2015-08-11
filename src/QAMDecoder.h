/*
 * QAMDecoder.h
 *
 *  Created on: Dec 8, 2014
 *      Author: fede
 */
#include "BpNode.h"
#include "SymbolNode.h"
#include <boost/numeric/ublas/matrix.hpp>

#ifndef QAMDECODER_H_
#define QAMDECODER_H_

class QAMDecoder {
public:
	template<typename T> int sign(T val) {
		return (val > T(0)) - (val < T(0));
	}
	static const int PI_ROW = 27;
	QAMDecoder(int k, int n, boost::numeric::ublas::matrix<int> matrix,
			int symbols, int passes);
	std::vector<int> decode(std::vector<double> phase, std::vector<double> quad,
			double n0);
	virtual ~QAMDecoder();
private:
	static const bool DEBUG = false;
	int passes;
	int symbols;
	int s;
	int k;
	int n;
	int p;
	boost::numeric::ublas::matrix<int> matrix;
	std::vector<SymbolNode*> received;
	std::vector<BpNode*> encoded;
	std::vector<BpNode*> parity;
	void forwardPassing();
	void backwardPassing();
	bool parityCheck();
	void initializeGraph();
	void resetGraph();
	std::vector<double> llr(int symbols, double received_p, double received_q,
			double n0);
};

#endif /* QAMDECODER_H_ */
