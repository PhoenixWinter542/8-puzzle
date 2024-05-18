#include "StateStorage.h"

int StateStorage::chooseName(node state)
{
	switch (nameOp) {
	case 0:
		return (int)state.cost;
		break;
	default:
	case 1:
		return (int)state.heur;
		break;
	case 2:
		return (int)state.heur + (int)state.cost;
		break;
	}
}

vector<node*> StateStorage::getBucket(int name)
{
	for (int i = 0; i < states.size(); i++) {
		if (states[i]->name == name)
			return states[i]->states;
	}

	return {};
}

vector<node*> StateStorage::getBucket(node state)
{
	return getBucket(chooseName(state));
}

void StateStorage::addState(node state)
{
	for (int i = 0; i < states.size(); i++) {
		if (states[i]->name == chooseName(state)) {
			states[i]->addState(state);
			return;
		}
	}

	for (int i = 0; i < states.size(); i++) {
		if (chooseName(state) < states[i]->name) {
			states.emplace(states.begin() + i, new StateBucket(chooseName(state)));
			states[i]->addState(state);
			return;
		}
	}

	states.push_back(new StateBucket(chooseName(state)));
	states[states.size() - 1]->addState(state);
}

void StateStorage::removeState(node state)
{
	for (int i = 0; i < states.size(); i++) {
		if (states[i]->name == chooseName(state)) {
			if (true == states[i]->removeState(state)) {	//removed the last node in the bucket, delete the bucket
				delete states[i];
				states.erase(states.begin() + i);
			}
			return;
		}
	}
}

void StateStorage::removeState(node state, int pos)
{
	for (int i = 0; i < states.size(); i++) {
		if (states[i]->name == chooseName(state)) {
			if (true == states[i]->removeState(pos)) {	//removed the last node in the bucket, delete the bucket
				delete states[i];
				states.erase(states.begin() + i);
			}
			return;
		}
	}
}

void StateStorage::clear()
{
	for (int i = 0; i < states.size(); i++) {
		states[i]->clear();
		delete states[i];
	}
	states.clear();
}

bool StateStorage::empty()
{
	return states.empty();
}

unsigned int StateStorage::size()
{
	int sum = 0;
	for (int i = 0; i < states.size(); i++) {
		sum += states[i]->size();
	}
	return sum;
}

node& StateStorage::operator[](int pos)
{
	unsigned int sum = 0;
	for (int i = 0; i < states.size(); i++) {
		if (pos < sum + states[i]->size()) {
			return states[i]->at(pos - sum);
		}
		sum += states[i]->size();
	}
	return states[0]->at(pos);	//Will throw error, that is the point
}

void StateStorage::pop_front()
{
	if (false == states.empty()) {
		if (true == states[0]->pop_front()) {
			delete states[0];
			states.erase(states.begin());
		}

	}
}

StateStorage::StateStorage()
{
	nameOp = 1;
}

StateStorage::StateStorage(int op)
{
	nameOp = op;
}

StateStorage::~StateStorage()
{
	clear();
}
