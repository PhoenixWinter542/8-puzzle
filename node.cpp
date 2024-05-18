#pragma once
#include <vector>
#include <math.h>
#include <cstdint>

#define intSize uint_fast8_t	//Defines the storage type for the values in a puzzle

using namespace std;

struct node {
	vector<vector<intSize>> puzzle;
	int row;
	int col;
	int prev;
	int cost;
	double heur;
	vector<int> path;

	inline bool sameState(node);
};

inline bool node::sameState(node compareWith)
{
	for (int j = 0; j < puzzle.size(); j++) {
		for (int k = 0; k < puzzle[j].size(); k++) {
			if (compareWith.puzzle[j][k] != puzzle[j][k]) {
				return false;
			}
		}
	}
	return true;

}
