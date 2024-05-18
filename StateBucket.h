#pragma once
#include "node.cpp"

class StateBucket {
public:
	int name;
	vector<node*> states;

	void addState(node);
	bool removeState(int);
	bool removeState(node);
	void clear();
	unsigned int size();
	bool pop_front();

	StateBucket(int);
	StateBucket(node);

	node& operator[](int);
	node& at(int);
};