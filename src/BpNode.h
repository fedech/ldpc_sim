#ifndef BPNODE_H_
#define BPNODE_H_

#include <vector>

class BpNode {

public:
	enum BpType {
		BP_VAR = 0, BP_CHECK = 1,
	};

	template<typename T> int llr2val(T val) {
		return (T(0) >= val);
	}
	template<typename T> int sign(T val) {
		return (val > T(0)) - (val < T(0));
	}

	BpNode(double initialValue, BpType type);
	virtual ~BpNode();
	void addNeighbor(BpNode *node);
	void sendMessages();
	double getValue();
	void setValue(double newValue);
	void resetNode();
private:
	struct neighbor {
		BpNode* node;
		double message;
	};
	static const bool MINSUM = false;
	std::vector<neighbor> neighbors;
	BpType type;
	double value;
	static const double MAX_LLR = 50;
	void message(BpNode *rec, double message);
	double findMessage(neighbor rec);
};

#endif /* BPNODE_H_ */
