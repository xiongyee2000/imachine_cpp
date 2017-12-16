#ifndef TRANSITION_H_HEADER_INCLUDED_BA47E601
#define TRANSITION_H_HEADER_INCLUDED_BA47E601

#include <string>
#include <functional>

namespace org {namespace jinsha{namespace imachine{
class State;
class StateMachine;
class StateMachineEngine;

class Transition
{
	friend class State;
    friend class StateMachine;
    friend class StateMachineEngine;

public:
	static const int INVALID_ID;

	const StateMachine& getStateMachine() const {return machine;};
    int getId() const {return id;};
    const std::string& getName() const {return name;};

protected:

private:
	Transition(StateMachine& machine, int id, int fromStateId, int ToStateId, const std::string& name = "");
	Transition();
	Transition(const Transition& transition);
	Transition& operator = (const Transition& transition);
	virtual ~Transition();

    int id;
    std::string name;
    StateMachine& machine;
    int fromStateId;
    int toStateId;
    State* fromState;
    State* toState;
    const void* action;
};

}}}//end of namespace

#endif /* TRANSITION_H_HEADER_INCLUDED_BA47E601 */
