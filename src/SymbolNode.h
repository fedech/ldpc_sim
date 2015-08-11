/*
 * SymbolNode.h
 *
 *  Created on: Jan 19, 2015
 *      Author: fede
 */

#ifndef SYMBOLNODE_H_
#define SYMBOLNODE_H_

#include <vector>
#include "BpNode.h"

class SymbolNode {

public:

	template<typename T> int sign(T val) {
		return (val > T(0)) - (val< T(0));
	}

	SymbolNode(double phase, double quad, int modSize);
	virtual ~SymbolNode();
	void addNeighbor(BpNode *node);
	void sendMessages();
	void setValue(double phase, double quad);
	void updateValue();
	void resetNode();
private:
	int modSize;
	struct neighbor{
		BpNode* node;
		double message;
	};
	static const bool MINSUM = false;
	std::vector<neighbor> neighbors;
	double phase;
	double quad;
	static const double MAX_LLR = 50;
	void message(BpNode *rec,double message);
	double findMessage(neighbor rec);
};
#endif /* SYMBOLNODE_H_ */
