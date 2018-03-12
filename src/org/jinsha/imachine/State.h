#ifndef STATE_H_HEADER_INCLUDED_BA47843C
#define STATE_H_HEADER_INCLUDED_BA47843C

#include <string>
#include <map>
#include <functional>

namespace org {namespace jinsha{namespace imachine{
class Transition;
class StateMachine;
class StateMachineEngine;

/**
 * Class definition of State.
 *
 * Notes:
 * The instance of State can only be created by
 * StateMachine::addState().
 */
class State
{
	friend class Transition;
    friend class StateMachine;
    friend class StateMachineEngine;

public:
    /**
     * The id of FINAL_STATE.
     *
     * Notes:
     * Every state machine must have exact 1 FINAL_STATE;
     * The value of FINAL_STATE_ID is -1.
     */
    static const int FINAL_STATE_ID;

    /**
     * The id of an invalid state.
     *
     * Notes:
     * The invalid state is used internally.
     * The value of INVALID_STATE_ID is -2.
     */
    static const int INVALID_STATE_ID;

    /**
     * The name of FINAL_STATE.
     *
     * Notes:
     * Every state machine must have exact 1 FINAL_STATE;
     * The value of FINAL_STATE_NAME is "FINAL".
     */
    static const std::string FINAL_STATE_NAME;

    /**
     * Get the state machine that this state belongs to.
     *
     * @return The state machine that this state belongs to.
     *
     * Notes:
     * A state must be created and managed by a state machine.
     */
	const StateMachine& getStateMachine() const {return machine;};

    /**
     * Get the sub state machine that this state holds.
     *
     * @return The sub state machine that this state holds, or
     * nullptr if it holds no sub state machine.
     */
	const StateMachine* getSubMachine() const {return subMachine;};

    /**
     * Get the id of this state.
     *
     * @return The id of this state.
     *
     * Notes:
     * The id must be unique within its enclosing state machine.
     */
    int getId() const {return id;};

    /**
     * Get the name of this state.
     *
     * @return The name of this state.
     *
     * Notes:
     * The name is not required to be unique within its enclosing
     * state machine, but is strongly recommended so.
     */
    const std::string& getName() const {return name;};

protected:

private:

	State(StateMachine& machine, int id, const std::string& name = "");
    State();
    State(const State& state);
    State& operator = (const State& state);
    virtual ~State();

    void addTrigger(int eventId, Transition* transition);

    int id;
    std::string name;
    StateMachine* subMachine;
    std::map<int, Transition*> triggers;
    StateMachine& machine;
    bool autoStartSubMachine;
    Transition* exitTransition;

    const void* entryAction;
    const void* exitAction;

};

}}}//end of namespace

#endif /* STATE_H_HEADER_INCLUDED_BA47843C */
