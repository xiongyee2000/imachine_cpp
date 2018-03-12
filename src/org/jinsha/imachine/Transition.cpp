#include "Transition.h"

namespace org {namespace jinsha{namespace imachine{

const int Transition::INVALID_TRANSITION_ID = -2;

Transition::Transition(StateMachine& machine, int id, int fromStateId, int ToStateId, const std::string& name) :
		machine(machine),
		id(id),
		name(name),
		fromStateId(fromStateId),
		toStateId(ToStateId),
		fromState(nullptr),
		toState(nullptr),
		action(nullptr)
{
}

Transition::~Transition()
{
}


}}}//end of namespace
