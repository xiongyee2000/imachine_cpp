#include "State.h"

#include "StateMachine.h"

using namespace std;

namespace org {namespace jinsha{namespace imachine{

const int State::INVALID_STATE_ID = -2;
const int State::FINAL_STATE_ID = -1;
const std::string State::FINAL_STATE_NAME = "FINAL";

State::State(StateMachine& machine, int id, const std::string& name) :
		machine(machine),
		id(id),
		name(name),
		subMachine(nullptr),
		autoStartSubMachine(true),
		exitTransition(nullptr),
		entryAction(nullptr),
		exitAction(nullptr)
{
}

State::~State()
{
//	if (subMachine != nullptr) delete subMachine;
}

void State::addTrigger(int eventId, Transition* transition)
{
	if (transition != nullptr) {
		triggers[eventId] = transition;
	}
}

}}}//end of namespace
