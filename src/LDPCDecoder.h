#ifndef LDPCDECODER_H_
#define LDPCDECODER_H_

#include <vector>
#include <map>
#include <fstream>
#include "BpNode.h"
#include <boost/numeric/ublas/matrix.hpp>

class LDPCDecoder {
public:
	struct symbol {
		double phase;
		double quad;
	};
	template<typename T> int sign(T val) {
		return (val > T(0)) - (val < T(0));
	}
	LDPCDecoder(int k, int n, boost::numeric::ublas::matrix<int> matrix,
			int passes, int symbols);
	std::vector<int> decode(std::vector<double> rec);
	std::vector<int> decode(std::vector<double> rec, boost::numeric::ublas::matrix<double> &errors, int counter, std::vector<int> uncoded);
	std::vector<double> deInterleave(std::vector<double> received);
	std::vector<double> deModLlr(std::vector<double> phase,
			std::vector<double> quad, double n0);
	virtual ~LDPCDecoder();
private:
	static const bool DEBUG = false;
	int passes;
	int k;
	int n;
	int p;
	int symbols;
	double energy;
	std::map<std::string,symbol> grayMap;
	boost::numeric::ublas::matrix<int> matrix;
	std::vector<std::vector<int> > checks;
	std::vector<BpNode*> received;
	std::vector<BpNode*> parity;
	void forwardPassing();
	void backwardPassing();
	bool parityCheck();
	void initializeGraph();
	void resetGraph();
	void initializeChecks();
	std::vector<double> llr(double received_p, double received_q, double n0);
	void initDemod();
};

#endif /* LDPCDECODER_H_ */
