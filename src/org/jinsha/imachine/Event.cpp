#include "Event.h"

//namespace org {namespace jinsha{namespace imachine{namespace engine {


//##ModelId=45B82BC400E9
Event::Event(int id, 
				   const std::string& name, 
				   TRANSITION_MAP_T** transitions, 
				   int transitionNum) :
		id(id),
		name(name),
		transitions(transitions),
		transitionNum(transitionNum)
{
}


//##ModelId=45B82CEB03DA
int Event::get_transitionId(int fromStateId) const
{
	for (int i = transitionNum - 1; i>= 0; i--) {
		if (fromStateId == transitions[i]->stateId) {
			return transitions[i]->transitionId;
		}
	}

	return INVALID_ID;
}






//##ModelId=45D3F8A7035B
Event::Event(int id, const std::string& name, int stateId, int transitionId) :
		id(id),
		name(name)
{
	TRANSITION_MAP_T** transitionTable = new TRANSITION_MAP_T*[1];
	transitionTable[0] = new TRANSITION_MAP_T;
	transitionTable[0]->stateId = stateId;
	transitionTable[0]->transitionId = transitionId;
	transitionNum = 1;
	transitions = transitionTable;
}






//##ModelId=460C1C7D0326
const std::string& Event::get_name() const
{
    return name;
}


//##ModelId=4618EEF30322
Event::~Event()
{
	for (int i = transitionNum; i > 0; i--) {
		delete transitions[i-1];
	}
	delete transitions;
}

