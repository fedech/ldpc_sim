/*
 * Simulation.cpp
 *
 *  Created on: Nov 29, 2014
 *      Author: fede
 */

#include <iostream>
#include <fstream>
#include <string>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include "LDPCEncoder.h"
#include "LDPCDecoder.h"

int main() {
	/*
	 * CODES:
	 * 6/3 (toy matrix)
	 * 648/324,432,486,540
	 * 1296/648,864,972,1080
	 * 1944/972,1296,1458,1620
	 */

	//Code parameters
	int n = 648;
	int k = 432;
	int p = n - k;
	std::string file = "n648r23";

	//Modulation and decoding parameters
	int passes = 50;
	bool qam = true;
	int symbols = 4;

	//Simulation parameters
	double step = 0.25;
	int minsnr = 0;
	double maxsnr = 4.1;
	int maxPackets = 1E6;
	int minErrors = 100;
	int printInterval = maxPackets / 1000;

	//Input/output parameters
	std::string matrixFolder = "./matrices/";
	std::string extIn = ".csv";
	std::string resultsFolder = "./results/";
	std::string extOut = ".dat";
	std::string encoding = matrixFolder + "enc" + file + extIn;
	std::string decoding = matrixFolder + "dec" + file + extIn;
	std::stringstream ss;
	if (qam) {
		ss << "qam" << symbols;
	}
	ss << "_" << passes;
	std::string output = resultsFolder + file + ss.str() + extOut;

	//Input and output initialization
	std::ifstream enc;
	enc.open(encoding.c_str());
	std::ifstream dec;
	dec.open(decoding.c_str());
	std::ofstream out;
	out.open(output.c_str());
	out << "#LDPC, n = " << n << ", k = " << k << "\n";
	out << "#SNR (dB) - BER - PER\n";

	//Vector and matrix creation
	std::vector<double> snrdb;
	std::vector<int> errors;
	std::vector<int> pktErrors;
	std::vector<int> packets;
	boost::numeric::ublas::matrix<int> parityMatrix(p, n);
	boost::numeric::ublas::matrix<int> encodingMatrix(n, k);

	//Encoding and decoding systematic matrix reading
	for (int i = 0; i < p; i++) {
		for (int j = 0; j < n; j++) {
			dec >> parityMatrix(i, j);
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < k; j++) {
			enc >> encodingMatrix(i, j);
		}
	}

	//Vector initialization
	for (double snr = minsnr; snr < maxsnr; snr += step) {
		snrdb.push_back(snr);
		errors.push_back(0);
		pktErrors.push_back(0);
		packets.push_back(0);
	}

	//Gaussian RNG initialization
	boost::mt19937 rng = boost::mt19937(time(0));
	boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > generator(
			rng, boost::normal_distribution<>());

	//Encoder and decoder creation
	LDPCEncoder *encoder = new LDPCEncoder(k, n, encodingMatrix, symbols);
	LDPCDecoder *decoder = new LDPCDecoder(k, n, parityMatrix, passes, symbols);

	std::cout << std::time(NULL) << "Created!\n";

	boost::numeric::ublas::matrix<double> results(snrdb.size(), 26);

	for (int i = 0; i < snrdb.size(); i++) {
		for (int j = 0; j < 26; j++) {
			results(i, j) = 0;
		}
	}

	//Simulation cycle
	for (int packet = 0; packet < maxPackets; packet++) {
		if (packet % printInterval == 0) {
			std::cout << packet << " " << std::time(NULL) << "\n";
		}
		std::vector<int> uncoded;
		std::vector<double> w;
		std::vector<double> w_q;

		//Noise and input sequence generation
		for (int i = 0; i < n; i++) {
			w.push_back(generator());
			if (qam) {
				w_q.push_back(generator());
			}
		}
		for (int i = 0; i < k; i++) {
			uncoded.push_back(rand() % 2);
		}

		//Encoding
		std::vector<int> coded = encoder->encode(uncoded);
		int counter = 0;

		//Packet sending and decoding
		for (double snr = minsnr; snr < maxsnr; snr += step) {
			if (packets.at(counter) == 0) {
				double n0 = 1 / pow(10, snr / 10) / log2(symbols) / 2;

				std::vector<double> received;
				std::vector<LDPCEncoder::symbol> modulated;

				if (!qam) {
					//Channel simulation (AWGN)
					for (int i = 0; i < n; i++) {
						double rec = 2 * coded.at(i) - 1 + w.at(i) * sqrt(n0);
						received.push_back(rec);
					}

					//LLR computation
					for (int i = 0; i < n; i++) {
						double llr = -2 * received.at(i) / n0;
						received.at(i) = llr;
					}
				} else {
					modulated = encoder->modulate(coded);
					std::vector<double> phase;
					std::vector<double> quad;
					//Channel simulation (AWGN)
					for (int i = 0; i < modulated.size(); i++) {
						phase.push_back(
								modulated.at(i).phase + w.at(i) * sqrt(n0));
						quad.push_back(
								modulated.at(i).quad + w_q.at(i) * sqrt(n0));
					}

					//LLR computation
					received = decoder->deModLlr(phase, quad, n0);
				}

				while (received.size() > n) {
					received.pop_back();
				}

				//Decoding
				//std::vector<int> decoded = decoder->decode(received,results,counter,uncoded);
				std::vector<int> decoded = decoder->decode(received);
				//Error checking
				int error = 0;
				bool correct = true;

				for (int i = 0; i < k; i++) {
					if (decoded.at(i) != coded.at(i)) {
						if (correct) {
							correct = false;
							pktErrors.at(counter)++;}
						error++;
					}
				}
				errors.at(counter) += error;

				if (pktErrors.at(counter) > minErrors) {
					packets.at(counter) = packet + 1;
				}

			}
			counter++;
		}
	}

	std::cout << std::time(NULL) << "Finished!\n";

	//Output calculation
	int counter = 0;
	for (double snr = minsnr; snr < maxsnr; snr += step) {
		double nPackets = maxPackets;
		if (packets.at(counter) != 0) {
			nPackets = packets.at(counter);
		}
		double per = pktErrors.at(counter) / nPackets;
		double ber = errors.at(counter) / (nPackets * k);
		out << snr << " " << ber << " " << per << "\n";
		counter++;
	}

//	std::ofstream outres;
//	std::string resFile="results.txt";
//	outres.open(resFile.c_str());
//
//	for (int i=0;i<snrdb.size();i++){
//		for(int j=1;j<26;j++){
//			outres<<results(i,j)/(double)packets.at(i)<<" ";
//		}
//		outres<<"\n";
//	}

	//Cleanup
	enc.close();
	dec.close();
	out.close();
	delete encoder;
	delete decoder;

}

