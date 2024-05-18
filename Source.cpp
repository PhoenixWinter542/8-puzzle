#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <time.h>
#include <tuple>
#include <chrono>
#include <future>
#include <string>
#include <algorithm>

#include "StateStorage.h"

using namespace std;

bool debug = false;

void printPuzzle(vector<vector<intSize>> puzzle);
tuple<int, int> findBlank(vector<vector<intSize>> puzzle);



class puzzles {
private:
	//Storage
	unsigned long expandedNodeCount;
	unsigned int maxQueueSize;

	int searchType;

	//node tracking
	//vector<node> frontier;
	//vector<node> closed;
	StateStorage frontier;
	StateStorage frontierHeur;
	StateStorage closed;

	//Heuristics
	double heuristic(vector<vector<intSize>> puzzle);
	int misplacedHeuristic(vector<vector<intSize>> puzzle);
	int manhattanHeuristic(vector<vector<intSize>> puzzle);
	double euclideanHeuristic(vector<vector<intSize>> puzzle);

	//Helper functions
	void addToFrontier(node toAdd);
	void nodeExpand(node current);
	void setup(vector<vector<intSize>> puzzle);
	bool testState(vector<node*>, node);

public:
	bool stateExists(node toAdd);
	void search(vector<vector<intSize>> puzzle);
	void search(vector<vector<intSize>> puzzle, int searchType);
	void printSteps(vector<int> path);

	puzzles();
	puzzles(int);
};

puzzles::puzzles() {
	frontier = StateStorage(2);
	frontierHeur = StateStorage(1);
	closed = StateStorage(1);
	searchType = 1;
}

puzzles::puzzles(int searchType) {
	if (0 == searchType)
		frontier = StateStorage(0);
	else
		frontier = StateStorage(2);

	frontierHeur = StateStorage(1);
	closed = StateStorage(1);
	this->searchType = searchType;
}

void puzzles::printSteps(vector<int> path) {
	cout << "Steps taken are: start";
	for (int i = 0; i < path.size(); i++) {
		cout << " -> ";
		string move = "";
		switch (path[i]) {
		case 0:
			move = "up";
			break;
		case 1:
			move = "down";
			break;
		case 2:
			move = "left";
			break;
		case 3:
			move = "right";
			break;
		}
		cout << move;
	}
}

tuple<int, int> findBlank(vector<vector<intSize>> puzzle) {
	for (int i = 0; i < puzzle.size(); i++) {
		for (int j = 0; j < puzzle[0].size(); j++) {
			if (0 == puzzle[i][j])
				return { i, j };
		}
	}
	return { -1, -1 };
}

void puzzles::setup(vector<vector<intSize>> puzzle) {
	expandedNodeCount = 0;
	maxQueueSize = 0;
	closed.clear();
	frontier.clear();
	frontierHeur.clear();
	node first;
	first.puzzle = puzzle;
	tuple<int, int> tmp = findBlank(puzzle);
	first.row = get<0>(tmp);
	first.col = get<1>(tmp);
	first.prev = -1;
	first.cost = 0;
	first.heur = heuristic(puzzle);
	addToFrontier(first);
}

bool compare(vector<node> puzzleList, node currentState) {
	for (int i = 0; i < puzzleList.size(); i++) {
		if (currentState.heur == puzzleList[i].heur) {		//Identical states should have identical heur values
			if (true == puzzleList[i].sameState(currentState))
				return true;
		}
	}
	return false;
}

bool puzzles::testState(vector<node*> states, node toAdd)
{
	for (int i = 0; i < states.size(); i++) {
		if (toAdd.heur == states[i]->heur) {		//Identical states should have identical heur values
			if (true == states[i]->sameState(toAdd))
				return true;
		}
	}
	return false;
}

bool puzzles::stateExists(node toAdd) {
	if (true == testState(frontierHeur.getBucket(toAdd), toAdd))
		return true;
	return testState(closed.getBucket(toAdd), toAdd);
}

void puzzles::search(vector<vector<intSize>> puzzle, int searchType) {
	this->searchType = searchType;
	search(puzzle);
	searchType = 1;
}

void puzzles::search(vector<vector<intSize>> puzzle) {
	setup(puzzle);

	while (true) {
		if (true == frontier.empty()) {
			cout << "\n\nFailed\n\n";
			cout << "To solve this problem the search algorithm expanded a total of " << expandedNodeCount << " nodes." << endl;
			cout << "The maximum number of nodes in the queue at any one time : " << maxQueueSize << endl;
			return;
		}
		if (0 == frontier[0].heur) {
			cout << "\n\nFOUND\n\n" << endl;
			cout << "To solve this problem the search algorithm expanded a total of " << expandedNodeCount << " nodes." << endl;
			cout << "The maximum number of nodes in the queue at any one time : " << maxQueueSize << endl;
			cout << "The depth of the goal node was " << frontier[0].path.size() << endl;
			printPuzzle(frontier[0].puzzle);
			printSteps(frontier[0].path);
			cout << "\n\n";
			return;
		}
		if (0 == expandedNodeCount++ % 100) {
			cout << ".";
		}
		if (0 == expandedNodeCount % 10000) {
			cout << "\n\n";
			cout << "Nodes Expanded	" << expandedNodeCount << " nodes." << endl;
			cout << "Max Queue:	" << maxQueueSize << endl;
			cout << "Heur:		" << frontier[0].heur << endl;
			cout << "Current Depth:	" << frontier[0].path.size() << "\n\n";
			printPuzzle(frontier[0].puzzle);
			cout << "\n";
		}
		
		nodeExpand(frontier[0]);
	}
}

void puzzles::addToFrontier(node toAdd) {
	if (frontier.size() + 1 > maxQueueSize)
		maxQueueSize = frontier.size() + 1;
	frontier.addState(toAdd);
	frontierHeur.addState(toAdd);
}

bool test(puzzles *puz, node newNode) {
	return puz->stateExists(newNode);
}

void puzzles::nodeExpand(node current) {
	current.cost++;
	closed.addState(frontier[0]);
	frontierHeur.removeState(frontier[0]);
	frontier.pop_front();	//remove node being expanded from frontier
	vector<node> toAdd;
	//Move blank up		prev = 0
	if (1 != current.prev && 0 != current.row) {
		node up = current;
		up.row--;
		up.prev = 0;
		int tmp = up.puzzle[up.row][up.col];
		up.puzzle[up.row][up.col] = 0;
		up.puzzle[current.row][up.col] = tmp;
		up.heur = heuristic(up.puzzle);
		up.path.push_back(0);
		toAdd.push_back(up);
	}

	//Move blank down	prev = 1
	if (current.prev != 0 && current.puzzle.size() - 1 != current.row) {
		node down = current;
		down.row++;
		down.prev = 1;
		int tmp = down.puzzle[down.row][down.col];
		down.puzzle[down.row][down.col] = 0;
		down.puzzle[current.row][down.col] = tmp;
		down.heur = heuristic(down.puzzle);
		down.path.push_back(1);
		toAdd.push_back(down);
	}

	//Move blank left	prev = 2
	if (current.prev != 3 && 0 < current.col) {
		node left = current;
		left.col--;
		left.prev = 2;
		int tmp = left.puzzle[left.row][left.col];
		left.puzzle[left.row][left.col] = 0;
		left.puzzle[left.row][current.col] = tmp;
		left.heur = heuristic(left.puzzle);
		left.path.push_back(2);
		toAdd.push_back(left);
	}

	//Move blank right	prev = 3
	if (current.prev != 2 && current.puzzle.size() - 1 > current.col) {
		node right = current;
		right.col++;
		right.prev = 3;
		int tmp = right.puzzle[right.row][right.col];
		right.puzzle[right.row][right.col] = 0;
		right.puzzle[right.row][current.col] = tmp;
		right.heur = heuristic(right.puzzle);
		right.path.push_back(3);
		toAdd.push_back(right);
	}
	
	//vector<future<bool>> results;
	vector<bool> results;
	for (int i = 0; i < toAdd.size(); i++) {
		//testing(frontier, closed, toAdd[i]);
		//results.push_back(async(launch::async, testing, frontier, closed, toAdd[i]));
		results.push_back(stateExists(toAdd[i]));
	}
	for (int i = 0; i < toAdd.size(); i++) {
		if (false == results[i])
			addToFrontier(toAdd[i]);
	}
}

double puzzles::heuristic(vector<vector<intSize>> puzzle) {
	switch (searchType) {
	case 0:	//Uniform Cost
		return euclideanHeuristic(puzzle);		//Not used for search, just to reduce stateExists runtime
		break;
	default:
	case 1:	//Misplaced Tile
		return misplacedHeuristic(puzzle);
		break;
	case 2:	//Euclidean Distance
		return euclideanHeuristic(puzzle);
		break;
	case 3:	//Manhattan Distance
		return manhattanHeuristic(puzzle);
		break;
	}
}

int puzzles::manhattanHeuristic(vector<vector<intSize>> puzzle) {
	int rowSize = puzzle.size();
	int expected = 1;
	int incorrect = 0;
	for (int i = 0; i < puzzle.size(); i++) {
		for (int j = 0; j < puzzle[0].size(); j++) {
			if (puzzle[i][j] != expected++) {
				//Find how many slides it would take to get to correct position
				int tmp = puzzle[i][j] - 1;
				if (-1 == tmp && rowSize - 1 == i && rowSize - 1 == j)	//Blank is in correct position
					break;
				int correctCol = tmp % rowSize;
				int correctRow = tmp / rowSize;
				incorrect += abs(correctRow - i);
				incorrect += abs(correctCol - j);
			}
		}
	}
	return incorrect;
}

int puzzles::misplacedHeuristic(vector<vector<intSize>> puzzle) {
	int rowSize = puzzle.size();
	int expected = 1;
	int incorrect = 0;
	for (int i = 0; i < puzzle.size(); i++) {
		for (int j = 0; j < puzzle[0].size(); j++) {
			if (puzzle[i][j] != expected++) {
				incorrect++;
			}
		}
	}
	if (0 == puzzle[puzzle.size() - 1][puzzle[0].size() - 1])	//The blank is correctly placed at in the bottom left corner
		incorrect--;
	return incorrect;
}

double puzzles::euclideanHeuristic(vector<vector<intSize>> puzzle) {
	int rowSize = puzzle.size();
	int expected = 1;
	double incorrect = 0;
	for (int i = 0; i < puzzle.size(); i++) {
		for (int j = 0; j < puzzle[0].size(); j++) {
			if (puzzle[i][j] != expected++) {
				//Find how many slides it would take to get to correct position
				int tmp = puzzle[i][j] - 1;
				if (-1 == tmp && rowSize - 1 == i && rowSize - 1 == j)	//Blank is in correct position
					break;
				double correctCol = tmp % rowSize;
				double correctRow = tmp / rowSize;
				incorrect += sqrt(pow(correctRow - (double)i, 2) + pow(correctCol - (double)j, 2));
			}
		}
	}
	return incorrect;
}

void scramblePuzzle(vector<vector<intSize>> &puzzle){
	//scramble puzzle
	srand(time(0));
	auto tmp = findBlank(puzzle);
	int row = get<0>(tmp), col = get<1>(tmp);	//Find the blank to move
	int size = puzzle.size(); //All puzzles are squares
	for (int i = 0; i < 100000; i++) {
		switch (rand() % 4) {
		case 0:	//up
			if (0 < row) {
				int tmp = puzzle[row - 1][col];
				puzzle[row - 1][col] = puzzle[row][col];
				puzzle[row][col] = tmp;
				row--;
			}
			break;
		case 1:	//down
			if (size - 1 > row) {
				int tmp = puzzle[row + 1][col];
				puzzle[row + 1][col] = puzzle[row][col];
				puzzle[row][col] = tmp;
				row++;
			}
			break;
		case 2:	//left
			if (0 < col) {
				int tmp = puzzle[row][col - 1];
				puzzle[row][col - 1] = puzzle[row][col];
				puzzle[row][col] = tmp;
				col--;
			}
			break;
		case 3:	//right
			if (size - 1 > col) {
				int tmp = puzzle[row][col + 1];
				puzzle[row][col + 1] = puzzle[row][col];
				puzzle[row][col] = tmp;
				col++;
			}
			break;
		}
	}
}

//Creates and scrambles a puzzle of a given size
vector<vector<intSize>> createPuzzle(int size) {
	int colCount = sqrt(size + 1);	//Rounds down to nearest square puzzle size

	//create solved puzzle
	vector<vector<intSize>> puzzle;
	int count = 1;
	for (int i = 0; i < colCount; i++) {
		vector<intSize> row;
		for (int j = 0; j < colCount; j++) {
			row.push_back(count++);
		}
		puzzle.push_back(row);
		row.clear();
	}
	puzzle[colCount - 1][colCount - 1] = 0;		//0 == blank tile

	return puzzle;
}


void printPuzzle(vector<vector<intSize>> puzzle) {
	for (int i = 0; i < puzzle.size(); i++) {
		for (int j = 0; j < puzzle[0].size(); j++) {
			int val = puzzle[i][j];
			if (0 == val)
				cout << "\t*";
			else
				cout << "\t" << val;

		}
		cout << "\n\n";
	}
}


void givenTests() {
	puzzles p = puzzles();
	vector<vector<intSize>> puzzle;
	puzzle = {		//Trivial
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 0}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nTrivial:\n";
	printPuzzle(puzzle);
	auto start = chrono::steady_clock::now();
	debug = false;
	p.search(puzzle, 3);
	auto finish = chrono::steady_clock::now();
	auto duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	puzzle = {		//Very Easy
		{1, 2, 3},
		{4, 5, 6},
		{7, 0, 8}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nVery Easy:\n";
	printPuzzle(puzzle);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(puzzle, 3);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;


	puzzle = {		//Easy
		{1, 2, 0},
		{4, 5, 3},
		{7, 8, 6}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nEasy:\n";
	printPuzzle(puzzle);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(puzzle, 3);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	puzzle = {		//Doable
		{0, 1, 2},
		{4, 5, 3},
		{7, 8, 6}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nDoable:\n";
	printPuzzle(puzzle);
	start = chrono::steady_clock::now();
	debug = true;
	p.search(puzzle, 3);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << endl;

	puzzle = {		//Oh Boy
		{8, 7, 1},
		{6, 0, 2},
		{5, 4, 3}
	};
	cout << "------------------------------------------------------------------------------------------------\n\nOh Boy:\n";
	printPuzzle(puzzle);
	start = chrono::steady_clock::now();
	debug = false;
	p.search(puzzle, 3);
	finish = chrono::steady_clock::now();
	duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
	cout << "Duration: " << duration << "\n\n";
}

int stringToInt(string input){
	vector<int> converted;
	int tmp = 0;
	//Check for any non-numeric characters
	for(int i = 0; i < input.size(); i++){
		if(false == isdigit(input[i]))
			return -1;
	}
	return stoi(input);
}

/*Generates a normalized puzzle from user input
	- lowest value is treated as blank
	- duplicates are treated as sequential numbers
	- assumes end state is numbers in ascending order from top left
*/
vector<vector<intSize>> userPuzzle(){
	string input;
	vector<int> tiles;
	cout << "Enter your puzzle followed by a non-numeric symbol" << endl;
	while(true){
		cin >> input;
		int digit = stringToInt(input);
		if(-1 == digit)	//Non-digit character was entered, break loop
			break;
		tiles.push_back(digit);
	};

	//Rounds down to the nearest square puzzle
	int size = sqrt(tiles.size());
	tiles.resize(size * size);

	//create a normalized puzzle from user input
	vector<int> copy = tiles;
	sort(copy.begin(), copy.end());	//copy is used to find the normalized value from the unsorted tiles vector
	vector<int> normalized;
	for(int i = 0; i < tiles.size(); i++){
		auto iter = find(copy.begin(), copy.end(), tiles[i]);	//find the position of the tile in the sorted list
		int val = distance(copy.begin(), iter);
		normalized.push_back(val);
	}

	int pos = 0;
	vector<vector<intSize>> puzzle;
	for(int col = 0; col < size; col++){
		vector<intSize> rowVec;
		for(int row = 0; row < size; row++){
			rowVec.push_back(normalized[pos++]);
		}
		puzzle.push_back(rowVec);
		rowVec.clear();
	}
	return puzzle;
}



int main() {

	string input;
	int choice;
	puzzles p = puzzles();
	vector<vector<intSize>> puzzle;
	do{
		cout << "1. Solve random puzzle" << endl;
		cout << "2. Solve user puzzle" << endl;
		cout << "3. Solve premade puzzles" << endl;
		cout << "\nEnter anything else to end program" << endl;
		cin >> input;
		choice = stringToInt(input);
		if(3 == choice){
			givenTests();
		}
		else{
			if(1 == choice){
				puzzle = createPuzzle(8);
				scramblePuzzle(puzzle);
			}
			else if(2 == choice){
				puzzle = userPuzzle();
				printPuzzle(puzzle);
			}
			
			printPuzzle(puzzle);
	
			auto start = chrono::steady_clock::now();
			p.search(puzzle, 3);
			auto finish = chrono::steady_clock::now();
			auto duration = chrono::duration_cast<chrono::duration<double>>(finish - start).count();
			cout << "Duration: " << duration << "\n\n";
		}
	}
	while(choice != -1);

	return 0;
}