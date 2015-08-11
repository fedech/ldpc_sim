#include "LDPCDecoder.h"
#include "QAMDecoder.h"
#include <stdexcept>

LDPCDecoder::LDPCDecoder(int k, int n,
		boost::numeric::ublas::matrix<int> matrix, int passes, int symbols) {
	this->k = k;
	this->n = n;
	this->passes = passes;
	this->matrix = matrix;
	this->symbols = symbols;
	energy = (symbols - 1) * 2 / 3;
	p = n - k;
	initializeGraph();
	initializeChecks();
	if (symbols > 2) {
		initDemod();
	}
}

std::vector<int> LDPCDecoder::decode(std::vector<double> llr) {
	if (n != llr.size()) {
		std::stringstream str;
		str << "Invalid input size";
		throw std::invalid_argument(str.str());
	}

	//LLR variable node initialization
	for (int i = 0; i < n; i++) {
		received.at(i)->setValue(llr.at(i));
		if (DEBUG) {
			std::cout << llr.at(i) << " ";
		}
	}
	if (DEBUG) {
		std::cout << " LLR\n";
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
		backwardPassing();
		if (DEBUG) {
			for (int i = 0; i < n; i++) {
				std::cout << received.at(i)->getValue() << " ";
			}
			std::cout << "LLR (var)\n";
		}
		check = parityCheck();
		pass += 2;
	}

	//Decoded vector
	std::vector<int> decoded;
	for (int i = 0; i < n; i++) {
		double in = received.at(i)->getValue();
		decoded.push_back(received.at(i)->llr2val(in));
	}
	resetGraph();

	return decoded;
}

void LDPCDecoder::forwardPassing() {
	for (int i = 0; i < n; i++) {
		received.at(i)->sendMessages();
	}
}

void LDPCDecoder::backwardPassing() {
	for (int i = 0; i < p; i++) {
		parity.at(i)->sendMessages();
	}
}

bool LDPCDecoder::parityCheck() {
	for (int i = 0; i < p; i++) {
		std::vector<int> check = checks.at(i);
		int sum = 0;
		for (int j = 0; j < check.size(); j++) {
			int index = check.at(j);
			sum += received.at(index)->llr2val(received.at(index)->getValue());
		}
		if (sum % 2 != 0) {
			return false;
		}
	}
	return true;
}

void LDPCDecoder::initializeGraph() {
	//Left and right side nodes initialization
	for (int i = 0; i < n; i++) {
		received.push_back(new BpNode(0, BpNode::BP_VAR));
	}
	for (int i = 0; i < p; i++) {
		parity.push_back(new BpNode(0, BpNode::BP_CHECK));
	}

	//Right side neighbors
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < n; j++) {
			if (matrix(i, j) != 0) {
				parity.at(i)->addNeighbor(received.at(j));
			}
		}
	}

	//Left side neighbors
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < p; i++) {
			if (matrix(i, j) != 0) {
				received.at(j)->addNeighbor(parity.at(i));
			}
		}
	}
}

void LDPCDecoder::resetGraph() {
//Reset left side nodes
	for (int i = 0; i < n; i++) {
		received.at(i)->resetNode();
	}

//Reset right side nodes
	for (int i = 0; i < p; i++) {
		parity.at(i)->resetNode();
	}
}

std::vector<double> LDPCDecoder::deInterleave(std::vector<double> received) {
	std::vector<double> deint;
	for (int i = 0; i < n / QAMDecoder::PI_ROW; i++) {
		for (int j = 0; j < QAMDecoder::PI_ROW; j++) {
			deint.push_back(received.at(i * QAMDecoder::PI_ROW + j));
		}
	}

	return deint;
}

std::vector<double> LDPCDecoder::deModLlr(std::vector<double> phase,
		std::vector<double> quad, double n0) {
	if (quad.size() != phase.size()) {
		throw std::invalid_argument("Invalid input size");
	}
	std::vector<double> llrs;

	int modSize = log2(symbols);

	//LLR calculation
	for (int s = 0; s < phase.size(); s++) {
		//LLR for the symbol
		std::vector<double> llr_symbol = llr(phase.at(s), quad.at(s), n0);

		for (int j = 0; j < modSize; j++) {
			llrs.push_back(llr_symbol.at(j));
		}
	}
	llrs = deInterleave(llrs);

	return llrs;
}

std::vector<double> LDPCDecoder::llr(double received_p, double received_q,
		double n0) {
	int modSize = log2(symbols);

	std::vector<double> llrs;
	std::vector<double> p1;
	std::vector<double> p0;
	for (int i = 0; i < modSize; i++) {
		llrs.push_back(0);
		p0.push_back(0);
		p1.push_back(0);
	}

	typedef std::map<std::string, symbol>::const_iterator it_type;
	for (it_type iterator = grayMap.begin(); iterator != grayMap.end();
			iterator++) {
		std::string coded = iterator->first;
		symbol noiseless = iterator->second;

		double dp = received_p - noiseless.phase;
		double dq = received_q - noiseless.quad;
		double d = dp * dp + dq * dq;
		double llr = exp(-d / (2 * n0));

		for (int i = 0; i < modSize; i++) {
			//sum the probability to the appropriate vector
			if (coded.at(i) == '1') {
				p1.at(i) += llr;
			} else {
				p0.at(i) += llr;
			}
		}
	}

	for (int i = 0; i < modSize; i++) {
		llrs.at(i) = log(p0.at(i) / p1.at(i));
	}

	return llrs;
}

void LDPCDecoder::initDemod() {
	std::string mapFile;
	bool mapped = false;
	if (symbols == 4) {
		mapFile = "./maps/gray4.csv";
		mapped = true;
	}
	if (symbols == 16) {
		mapFile = "./maps/gray16.csv";
		mapped = true;
	}
	if (symbols == 64) {
		mapFile = "./maps/gray64.csv";
		mapped = true;
	}
	if (!mapped) {
		return;
	}
	std::ifstream mapReader;
	mapReader.open(mapFile.c_str());
	for (int i = 0; i < symbols; i++) {
		std::string sequence;
		symbol symb;
		mapReader >> sequence >> symb.phase >> symb.quad;
		symb.phase /= sqrt(energy);
		symb.quad /= sqrt(energy);
		grayMap.insert(std::pair<std::string, symbol>(sequence, symb));
	}
}

void LDPCDecoder::initializeChecks() {
	for (int i = 0; i < p; i++) {
		std::vector<int> check;
		checks.push_back(check);
	}

	for (int i = 0; i < p; i++) {
		for (int j = 0; j < n; j++) {
			if (matrix(i, j) != 0) {
				checks.at(i).push_back(j);
			}
		}
	}
}

std::vector<int> LDPCDecoder::decode(std::vector<double> llr,
		boost::numeric::ublas::matrix<double> &errors, int counter,
		std::vector<int> uncoded) {
	if (n != llr.size()) {
		std::stringstream str;
		str << "Invalid input size";
		throw std::invalid_argument(str.str());
	}

	//LLR variable node initialization
	for (int i = 0; i < n; i++) {
		received.at(i)->setValue(llr.at(i));
		if (DEBUG) {
			std::cout << llr.at(i) << " ";
		}
	}
	if (DEBUG) {
		std::cout << " LLR\n";
	}

	//Message passing (stops when Hc=0 or after a max number of cycles)
	int pass = 0;
	bool check = false; //parityCheck();
	while (!check && pass < passes) {
		forwardPassing();
		if (DEBUG) {
			for (int i = 0; i < k; i++) {
				std::cout << parity.at(i)->getValue() << " ";
			}
			std::cout << "LLR (check)\n";
		}
		backwardPassing();
		if (DEBUG) {
			for (int i = 0; i < n; i++) {
				std::cout << received.at(i)->getValue() << " ";
			}
			std::cout << "LLR (var)\n";
		}
		//check = parityCheck();
		pass += 2;

		if (!parityCheck()) {
			errors(counter, pass / 2)++;

			}
		else {
			for (int i = 0; i < k; i++) {
				double in = received.at(i)->getValue();
				if (received.at(i)->llr2val(in) != uncoded.at(i)) {
					errors(counter, pass / 2)++;break
;				}
			}
		}
		errors(0, 0)++;}

		//Decoded vector
	std::vector<int> decoded;
	for (int i = 0; i < n; i++) {
		double in = received.at(i)->getValue();
		decoded.push_back(received.at(i)->llr2val(in));
	}
	resetGraph();

	return decoded;
}

LDPCDecoder::~LDPCDecoder() {
	received.clear();
	parity.clear();
}
