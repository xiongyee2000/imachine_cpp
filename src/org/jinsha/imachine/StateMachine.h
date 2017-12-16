#ifndef STATEMACHINE_H_HEADER_INCLUDED_BA465AF4
#define STATEMACHINE_H_HEADER_INCLUDED_BA465AF4

#include <string>
#include <map>
#include "State.h"
#include "Transition.h"

namespace org {namespace jinsha{namespace imachine{
class State;
class Transition;
class Trigger;
class StateMachineEngine;

class StateMachine {
friend class StateMachineEngine;

public:
	static bool validate(const StateMachine& machine);

    StateMachine(int id, const std::string& name = "");
    virtual ~StateMachine();

    int getId() const {return id;};
    State* getState(int stateId) const ;
    bool addState(int id, const std::string& name = "");

    /**
     * Note:
     * Automatic transition.
     * addTransition() must be called after all addState().
     */
    bool addTransition(int id, int fromStateId, int toStateId, const std::string& name = "");

    /**
     * Note:
     *
     * addTransition() must be called after the fromState and toState are added by calling addState().
     */
    bool addTransition(int id, int fromStateId, int toStateId, int eventId, const std::string& name = "");

    /**
     * Note:
     * setInitState() must be called after the state is added by calling addState().
     */
    bool setInitState(int stateId);

    /**
     * Note:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     * Return false when this is the final state
     */
    bool setSubMachine(int stateId, StateMachine* machine);

    /**
     * Note:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     * Return false when this is the final state
     * autoStart: Whether automatically start the sub-machine when this state is entered.
     */
    bool setSubMachine(int stateId, StateMachine* machine, bool autoStart);

    /**
     * Note:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     * Return false when this is the final state
     * exitTransitionId: The transition (in this state machine) to go to
     * when the sub-machine is shut down.
     */
    bool setSubMachine(int stateId, StateMachine* machine, int exitTransitionId);

    /**
     * Note:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     * Return false when this is the final state
     * autoStart: Whether automatically start the sub-machine when this state is entered.
     * exitTransitionId: The transition (in this state machine) to go to
     * when the sub-machine is shut down.
     */
    bool setSubMachine(int stateId, StateMachine* machine, bool autoStart, int exitTransitionId);

    void removeAllStates();
    void removeAllTransitions();

private:
    static const int AUTO_TRANSITION_EVENT;
    StateMachine();
    StateMachine(const StateMachine& stateMachine);
    StateMachine& operator = (const StateMachine& stateMachine);

    bool doSetSubMachine(int stateId, StateMachine* machine, bool* autoStart, int* exitTransitionId);
    bool doAddTransition(int id, int fromStateId, int toStateId, int* eventIdPtr, const std::string& name = "");

    int id;
    std::string name;
    State* initState;
    std::map<int, State*> states;
    std::map<int, Transition*> transitions;
    StateMachineEngine* engine;
};

}}}//end of namespace

#endif /* STATEMACHINE_H_HEADER_INCLUDED_BA465AF4 */
