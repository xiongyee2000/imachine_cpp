//#include <assert.h>

#include "StateMachine.h"

//##ModelId=45B9D2740022
StateMachine::StateMachine(int id, 
								 const char* name,
								 State** states, 
								 int stateNum, 
								 Transition** transitions,
								 int transitionNum, 
								 Event** events, 
								 int eventNum, 
								 int initState)
{
	this->id = id;
	this->name = name;
	this->states = states;
	this->stateNum = stateNum;
	this->transitions = transitions;
	this->transitionNum = transitionNum;
	this->events = events;
	this->eventNum = eventNum;
	this->initState = initState;
}



































