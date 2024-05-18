#include "StateBucket.h"

void StateBucket::addState(node state)
{
	for (int i = 0; i < states.size(); i++) {
		if (state.heur < states[i]->heur) {
			node* storeMe = new node;
			*storeMe = state;
			states.emplace(states.begin() + i, storeMe);
			return;
		}
	}

	node* storeMe = new node;
	*storeMe = state;
	states.push_back(storeMe);
}

bool StateBucket::removeState(int pos)
{
	node* deleteMe = states[pos];
	states.erase(states.begin() + pos);
	delete deleteMe;
	return states.empty();
}

bool StateBucket::removeState(node toRemove)
{
	for (int i = 0; i < states.size(); i++) {
		if (toRemove.cost == states[i]->cost && toRemove.heur == states[i]->heur) {
			if (true == states[i]->sameState(toRemove)) {
				delete states[i];
				states.erase(states.begin() + i);
				break;
			}
		}
	}
	return states.empty();
}

StateBucket::StateBucket(int name)
{
	this->name = name;
}

StateBucket::StateBucket(node state)
{
	this->name = (int)state.heur;
	addState(state);
}

void StateBucket::clear()
{
	for (int i = 0; i < states.size(); i++) {
		delete states[i];
	}
	states.clear();
}

unsigned int StateBucket::size()
{
	return states.size();
}

bool StateBucket::pop_front()
{
	states.erase(states.begin());
	return states.empty();
}

node& StateBucket::operator[](int pos)
{
	return *states[pos];
}

node& StateBucket::at(int pos)
{
	return *states[pos];
}

