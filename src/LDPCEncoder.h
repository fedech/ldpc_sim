#ifndef LDPCENCODER_H_
#define LDPCENCODER_H_

#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include <boost/numeric/ublas/matrix.hpp>

class LDPCEncoder {
public:
	struct symbol {
		double phase;
		double quad;
	};
	LDPCEncoder(int k, int n, boost::numeric::ublas::matrix<int> matrix,int symbols);
	virtual ~LDPCEncoder();
	std::vector<int> encode(std::vector<int> uncoded);
	std::vector<symbol> modulate(std::vector<int> coded);
	std::vector<int> interleave(std::vector<int> coded);

private:
	int k;
	int n;
	int symbols;
	boost::numeric::ublas::matrix<int> matrix;
	std::map<std::string,symbol> grayMap;
	double energy;
	symbol modulateSymbol(std::vector<int> coded);
	void initModulator();
};

#endif /* LDPCENCODER_H_ */
