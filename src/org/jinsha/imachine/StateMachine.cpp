//#include <assert.h>
#include "State.h"
#include "Transition.h"
#include "StateMachine.h"

namespace org {namespace jinsha{namespace imachine{

const int StateMachine::AUTO_TRANSITION_EVENT = -1;

bool StateMachine::validate(const StateMachine& machine)
{
	return true;
}

StateMachine::StateMachine(int id, const std::string& name) :
	id(id),
	name(name),
	initState(nullptr),
	engine(nullptr)
{
}

StateMachine::~StateMachine()
{
	removeAllTransitions();
	removeAllStates();

	initState = nullptr;
}

State* StateMachine::getState(int stateId) const
{
	auto iter = states.find(stateId);
	if (iter != states.end()) {
		return iter->second;
	}
	return nullptr;
}

bool StateMachine::addState(int id, const std::string& name)
{
	if (id <= State::INVALID_STATE_ID) return false; //invalid state id
	auto iter = states.find(id);
	if (iter != states.end()) { //Do not add it if it already exists.
		return false;
	}
	State* state = new State(*this, id, name);
	states[id] = state;
	return true;
}

bool StateMachine::addTransition(int id, int fromStateId, int toStateId, int eventId, const std::string& name)
{
	return doAddTransition(id, fromStateId, toStateId, &eventId, name);
}

bool StateMachine::addTransition(int id, int fromStateId, int toStateId, const std::string& name)
{
	return doAddTransition(id, fromStateId, toStateId, nullptr, name);
}

bool StateMachine::doAddTransition(int id, int fromStateId, int toStateId, int* eventIdPtr, const std::string& name)
{
	if (id <= Transition::INVALID_ID) return false; //invalid id
	auto iter1 = transitions.find(id);
	if (iter1 != transitions.end()) { //Do not add it if it already exists.
		return false;
	}

	State* fromState = nullptr;
	auto iter2 = states.find(fromStateId);
	if (iter2 == states.end()) { //Do not add it if from-state doesn't exist.
		return false;
	} else {
		fromState = iter2->second;
	}

	State* toState = nullptr;
	auto iter3 = states.find(toStateId);
	if (iter3 == states.end()) { //Do not add it if to-state doesn't exist.
		return false;
	} else {
		toState = iter3->second;
	}

	if (fromState != nullptr && toState != nullptr) {
		Transition* transition = new Transition(*this, id, fromStateId, toStateId, name);
		transition->fromState = fromState;
		transition->toState = toState;
		if (eventIdPtr == nullptr) { //auto-transition
			fromState->addTrigger(AUTO_TRANSITION_EVENT, transition);
		} else {
			fromState->addTrigger(*eventIdPtr, transition);
		}
		transitions[id] = transition;
	} else {
		return false;
	}

	return true;
}

void StateMachine::removeAllStates()
{
	for (auto item : states) {
		delete item.second;
	}
	states.clear();
}

void StateMachine::removeAllTransitions()
{
	for (auto item : transitions) {
		delete item.second;
	}
	transitions.clear();
}

bool StateMachine::setInitState(int stateId)
{
	if (stateId <= State::INVALID_STATE_ID) return false; //invalid state id
	auto iter = states.find(stateId);
	if (iter != states.end()) {
		initState = iter->second;
	} else {
		return false;
	}
	return true;
}

bool StateMachine::setSubMachine(int stateId, StateMachine* machine)
{
	return doSetSubMachine(stateId, machine, nullptr, nullptr);
}

bool StateMachine::setSubMachine(int stateId, StateMachine* machine,  bool autoStart)
{
	return doSetSubMachine(stateId, machine, &autoStart, nullptr);
}

bool StateMachine::setSubMachine(int stateId, StateMachine* machine, int exitTransitionId)
{
	return doSetSubMachine(stateId, machine, nullptr, &exitTransitionId);
}

bool StateMachine::setSubMachine(int stateId, StateMachine* machine, bool autoStart, int exitTransitionId)
{
	return doSetSubMachine(stateId, machine, &autoStart, &exitTransitionId);
}

bool StateMachine::doSetSubMachine(int stateId, StateMachine* machine, bool* autoStart, int* exitTransitionId)
{
	if (stateId == State::FINAL_STATE_ID) return false; //FINAL_STATE not allowed to have sub state machine

	auto iter = states.find(stateId);
	if (iter == states.end()) return false;

	State* state = iter->second;

	if (state->subMachine != nullptr && state->subMachine->engine != nullptr) {
		return false; //Not allow to set sub-machine when it has been loaded into engine.
	} else {
		state->subMachine = machine;
		if (machine == nullptr) {
			//If machine is null, set autoStart to true,
			//and set exitTransitionId to Transition::INVALID_ID.
			state->autoStartSubMachine = true;
			state->exitTransition = nullptr;
		} else {
			//set auto start flag
			if (autoStart == nullptr) {
				state->autoStartSubMachine = true;
			} else {
				state->autoStartSubMachine = *autoStart;
			}

			//set exit transition
			if (exitTransitionId == nullptr) {
				state->exitTransition = nullptr;
			} else {
				auto iter = transitions.find(*exitTransitionId);
				if (iter != transitions.end() &&
					iter->second->fromState->id == stateId) {
					state->exitTransition = iter->second;
				}
			}
		}
	}
	return true;
}

}}}//end of namespace
