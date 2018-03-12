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

/**
 * Class definition of State Machine.
 *
 * Notes:
 * A Naked State Machine contains only a set of states and
 * a set of transitions but no more. This ensures it gives the
 * most fundamental elments of a state machine so that the
 * maximum independency and flexibility can be achieved.
 */
class StateMachine {
friend class StateMachineEngine;

public:
	/**
	 * Check if a state machine is valid. (Not implemented yet)
	 *
	 * @return True if value, false otherwise.
	 * (Currently always return true)
	 */
	static bool validate(const StateMachine& machine);

	/**
	 * Constructor
	 *
	 * @param id Id of the state machine
	 * @param name Name of the state machine
	 */
    StateMachine(int id, const std::string& name = "");

    /**
     * Destructor
     */
    virtual ~StateMachine();

    /**
     * Get the id of this state machine.
     *
     * @return The id of this state machine.
     */
    int getId() const {return id;};

    /**
     * Get the state instance by the given state id.
     *
     * @return The pointer to the state instance,
     *         or nullptr if it does not exist.
     */
    State* getState(int stateId) const ;

    /**
     * Create a state and add it into this state machine.
     *
	 * @param id Id of the state
	 * @param name Name of the state
	 * @return True if success, false otherwise.
	 *
	 * Notes:
	 * If a state with the given id already exists, it will
	 * return false.
     */
    bool addState(int id, const std::string& name = "");

    /**
     * Create a transition and add it into this state machine.
     *
	 * @param id Id of the transition
	 * @param fromStateId Id of the from-state of the transition
	 * @param toStateId Id of the to-state of the transition
	 * @param name Name of the transition
	 * @return True if success, false otherwise.
	 *
	 * Notes:
     * This method is for automatic transition (no event trigger needed);
	 * If a transition with the given id already exists, it will
	 * return false;
     * This method must be called after all addState().
     */
    bool addTransition(int id, int fromStateId, int toStateId, const std::string& name = "");

    /**
     * Create a transition and add it into this state machine.
     *
	 * @param id Id of the transition
	 * @param fromStateId Id of the from-state of the transition
	 * @param toStateId Id of the to-state of the transition
	 * @param eventId Id of the event to trigger this transition
	 * @param name Name of the transition
	 * @return True if success, false otherwise.
	 *
	 * Notes:
     * This method is for event-driven transition;
	 * If a transition with the given id already exists, it will
	 * return false;
     * This method must be called after all addState().
     */
    bool addTransition(int id, int fromStateId, int toStateId, int eventId, const std::string& name = "");

    /**
     * Set initial state of the state machine.
     *
	 * @param stateId Id of the initial state
     *
     * Notes:
     * This method must be called after the state is added by calling addState().
     */
    bool setInitState(int stateId);

    /**
     * Set sub state machine of a state.
     *
     * @param stateId The id of the state to be set on
     * @param machine The sub state machine to be set.
     * @return False when the state with the given stateId
     *         cannot be found or it is FINAL_STATE_ID, true
     *         otherwise.
     *
     * Notes:
     * This method is equivalent to setSubMachine(stateId, machine, true);
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     */
    bool setSubMachine(int stateId, StateMachine* machine);

    /**
     * Set sub state machine of a state.
     *
     * @param stateId The id of the state to be set on
     * @param machine The sub state machine to be set.
     * @param autoStart Whether automatically start the
     *        sub-machine when this state is entered.
     * @return False when the state with the given stateId
     *         cannot be found or it is FINAL_STATE_ID, true
     *         otherwise.
     *
     * Notes:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     */
    bool setSubMachine(int stateId, StateMachine* machine, bool autoStart);

    /**
     * Set sub state machine of a state.
     *
     * @param stateId The id of the state to be set on
     * @param machine The sub state machine to be set.
     * @param exitTransitionId The transition (in this state machine)
     *        to go to when the sub-machine is shut down.
     * @return False when the state with the given stateId
     *         cannot be found or it is FINAL_STATE_ID, true
     *         otherwise.
     *
     * Notes:
     * This method is equivalent to setSubMachine(stateId, machine, true, exitTransitionId);
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     */
    bool setSubMachine(int stateId, StateMachine* machine, int exitTransitionId);

    /**
     * Set sub state machine of a state.
     *
     * @param stateId The id of the state to be set on
     * @param machine The sub state machine to be set.
     * @param autoStart Whether automatically start the
     *        sub-machine when this state is entered.
     * @param exitTransitionId The transition (in this state machine)
     *        to go to when the sub-machine is shut down.
     * @return False when the state with the given stateId
     *         cannot be found or it is FINAL_STATE_ID, true
     *         otherwise.
     *
     * Notes:
     * This method shall be used when the state machine
     * is not loaded into any state machine engine.
     */
    bool setSubMachine(int stateId, StateMachine* machine, bool autoStart, int exitTransitionId);

    /**
     * Remove all states from the state machine.
     */
    void removeAllStates();

    /**
     * Remove all transitions from the state machine.
     */
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
