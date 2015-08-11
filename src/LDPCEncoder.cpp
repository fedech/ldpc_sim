#include "LDPCEncoder.h"
#include "QAMDecoder.h"

LDPCEncoder::LDPCEncoder(int k, int n,
		boost::numeric::ublas::matrix<int> matrix, int symbols) {
	this->k = k;
	this->n = n;
	this->matrix = matrix;
	this->symbols = symbols;
	energy = (symbols - 1) * 2 / 3;
	if (symbols > 2) {
		initModulator();
	}
}

std::vector<int> LDPCEncoder::encode(std::vector<int> uncoded) {
	if (uncoded.size() != k) {
		throw std::invalid_argument("Invalid input size");
	}
	std::vector<int> coded;
	for (int i = 0; i < k; i++) {
		coded.push_back(uncoded.at(i));
	}
	for (int i = k; i < n; i++) {
		int element = 0;
		for (int j = 0; j < k; j++) {
			element += matrix(i, j) * uncoded.at(j);
		}
		coded.push_back(element % 2);
	}
	return coded;
}

std::vector<LDPCEncoder::symbol> LDPCEncoder::modulate(std::vector<int> coded) {
	coded = interleave(coded);
	std::vector<LDPCEncoder::symbol> qam;
	int modSize = log2(symbols);
	for (int i = 0; i < coded.size(); i += modSize) {
		std::vector<int> next;
		for (int j = 0; j < modSize; j++) {
			if (i + j >= coded.size()) {
				break;
			}
			next.push_back(coded.at(i + j));
		}
		qam.push_back(modulateSymbol(next));
	}
	return qam;
}

std::vector<int> LDPCEncoder::interleave(std::vector<int> coded) {
	std::vector<int> interleaved;
	for (int i = 0; i < n / QAMDecoder::PI_ROW; i++) {
		for (int j = 0; j < QAMDecoder::PI_ROW; j++) {
			interleaved.push_back(coded.at(i * QAMDecoder::PI_ROW + j));
		}
	}

	return interleaved;
}

LDPCEncoder::symbol LDPCEncoder::modulateSymbol(std::vector<int> coded) {
	int modSize = log2(symbols);
	//Input check
	if (modSize < coded.size()) {
		throw std::invalid_argument("Invalid input size");
	}

	//Zero padding for incomplete symbols
	while (coded.size() < modSize) {
		coded.push_back(0);
	}

	std::stringstream bits;
	for (int i = 0; i < modSize; i++) {
		bits << coded.at(i);
	}
	return grayMap.at(bits.str());
}

void LDPCEncoder::initModulator() {
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

LDPCEncoder::~LDPCEncoder() {
}
