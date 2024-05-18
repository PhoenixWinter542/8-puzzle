#pragma once
#include "StateBucket.h"

class StateStorage {
private:
	vector<StateBucket*> states;
	int nameOp = 0;

	inline int chooseName(node);

public:
	vector<node*> getBucket(int);
	vector<node*> getBucket(node);
	void addState(node);
	void removeState(node);
	void removeState(node, int);
	void clear();
	bool empty();
	unsigned int size();

	node& operator[](int);
	void pop_front();

	StateStorage();
	StateStorage(int);
	~StateStorage();
};