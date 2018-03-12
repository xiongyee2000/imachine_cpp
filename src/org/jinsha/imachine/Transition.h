#ifndef TRANSITION_H_HEADER_INCLUDED_BA47E601
#define TRANSITION_H_HEADER_INCLUDED_BA47E601

#include <string>
#include <functional>

namespace org {namespace jinsha{namespace imachine{
class State;
class StateMachine;
class StateMachineEngine;

/**
 * Class definition of Transition.
 *
 * Notes:
 * The instance of Transition can only be created by
 * StateMachine::addTransition().
 */
class Transition
{
	friend class State;
    friend class StateMachine;
    friend class StateMachineEngine;

public:

    /**
     * The id of an invalid transition.
     *
     * Notes:
     * The invalid transition is used internally.
     * The value of INVALID_ID is -2.
     */
	static const int INVALID_TRANSITION_ID;


    /**
     * Get the state machine that this transition belongs to.
     *
     * @return The state machine that this transition belongs to.
     *
     * Notes:
     * A transition must be created and managed by a state machine.
     */
	const StateMachine& getStateMachine() const {return machine;};

    /**
     * Get the id of this transition.
     *
     * @return The id of this transition.
     *
     * Notes:
     * The id must be unique within its enclosing state machine.
     */
    int getId() const {return id;};

    /**
     * Get the name of this transition.
     *
     * @return The name of this transition.
     *
     * Notes:
     * The name is not required to be unique within its enclosing
     * state machine, but is strongly recommended so.
     */
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
