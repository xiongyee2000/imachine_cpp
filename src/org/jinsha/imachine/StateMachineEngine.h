#ifndef STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1
#define STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1

#include <stdio.h>
#include <map>
#include <list>
//#include <queue>
#include <functional>

namespace org {namespace jinsha{namespace imachine{

/**
 * Class definition of state machine engine.
 *
 * Notes:
 * The state machine engine is an engine that drives the state machine
 * from one state to another;
 * State actions can be hooked to state entry/exit dymanically;
 * Transition actions can be hooked to transitions dymanically;
 * Transition guard can be set onto a transition to determine
 * if a transition can pass through or not.
 */
class StateMachineEngine
{
public:
	/**
	 * error codes
	 */
    enum error_code_t { 
		ERR_SUCCESS = 0,
		ERR_INVALID_STATE_MACHINE = 1024,
		ERR_MACHINE_LOADED,
		ERR_MACHINE_NOT_LOADED,
		ERR_ENGINE_ALIVE,
		ERR_ENGINE_TERMINATED,
		ERR_TRANSITION_IN_PROCESS, 
		ERR_IMPROPER_TRANSITION, 
		ERR_INVALID_TRANSITION, 
		ERR_INVALID_EVENT, 
		ERR_UNACCEPTABLE_EVENT,
		ERR_RECURSIVE_TRANSITION,
		ERR_FORBIDDEN,
		ERR_UNKOWN
	};

	/**
	 * The definition for State Action. State Action is a callback function
	 * which is invoked when a state enters or exits.
	 *
	 * @param state The state to which this action attached.
	 * @return void
	 */
	typedef std::function<void (const StateMachineEngine* engine, const State* state)> StateAction;

	/**
	 * The definition for Transition Action. Transition Action is a callback function
	 * which is invoked when a Transition is triggered.
	 *
	 * @param Transition The Transition to which this action attached.
	 * @return void
	 */
	typedef std::function<void (const StateMachineEngine* engine, const Transition* transition)> TransitionAction;

	/**
	 * Constructor
	 *
	 * @param id The id of the state machine engine.
	 */
    StateMachineEngine(int id);

    /**
     * Destructor
     */
    virtual ~StateMachineEngine();

    /**
     * Get the id of this state machine engine.
     *
     * @return The id of this state machine engine.
     */
    int getId() const {return id;};


    /**
     * Get the state machine this engine is attached to.
     *
     * @return The instance of state machine which is loaded into
     * this engine.
     */
    StateMachine& getStateMachine() const {return *machine;};

    /**
     * Get current state of the state machine.
     *
     * @return The pointer to the current state, or nullptr if
     *         the state machine engine is not started.
     */
    State* getCurrentState() const {return activeState;};

    /**
     * Get parent engine of this engine. Suppose engine E1 is loaded
     * into state machine M1, while engine E2 is loaded into state machine
     * M2. If M2 is the sub state machine of M1, then E1 is called the
     * parent engine of E2, and E2 is the sub engine of E1.
     *
     * @return The pointer to the parent engine, or nullptr if
     *         this engine does not have a parent.
     */
    StateMachineEngine* getParent() const {return parent;};

    /**
     * Get the sub engine of the state machine by the given state id.
     *
     * @return The sub engine of the state machine by the given state
     *         id, or nullptr if:
     *         The state machine with the id cannot be found;
     *         The state machine with the id holds no sub state machine.
     */
    StateMachineEngine* getSubEngine(int stateId) const;

    /**
     * If the state machine engine is started. When an engine is started,
     * the state machine will stay on a state other than FINAL.
     *
     * @return True if started, or false otherwise.
     */
    bool isStarted();

    /**
     * Load state machine into this engine.
     *
     * @param stateMachine The state machine to be loaded.
     * @return ERR_SUCCESS if successful, or the corresponding
     *         error code.
     */
    int loadStateMachine(StateMachine& stateMachine);


    /**
     * Unload state machine into this engine.
     *
     * @return ERR_SUCCESS if successful, or the corresponding
     *         error code.
     */
    int unloadStateMachine();

    /**
     * Set state entry action for a state.
     *
     * @param stateId The id of the state to be set on
     * @param action The action to be set
     * @return True if successful, false otherwise
     *
	 * Notes:
	 * If the stateId is FINAL_STATE_ID or invalid, it will
	 * return false;
	 * If a state with the given id does not exist, it will
	 * return false;
	 * Use action = nullptr to remove previously set action.
     */
    bool setStateEntryAction(int stateId, const StateAction* action);

    /**
     * Set state exit action for a state.
     *
     * @param stateId The id of the state to be set on
     * @param action The action to be set
     * @return True if successful, false otherwise
     *
	 * Notes:
	 * If the stateId is FINAL_STATE_ID or invalid, it will
	 * return false;
	 * If a state with the given id does not exist, it will
	 * return false;
	 * Use action = nullptr to remove previously set action.
     */
    bool setStateExitAction(int stateId, const StateAction* action);

    /**
     * Set transition action for a state.
     *
     * @param transitionId The id of the transition to be set on
     * @param action The action to be set
     * @return True if successful, false otherwise
     *
	 * Notes:
	 * If the transitionId is INVALID_TRANSITION_ID, it will
	 * return false;
	 * If a transition with the given id does not exist, it will
	 * return false;
	 * Use action = nullptr to remove previously set action.
     */
    bool setTransitionAction(int transitionId, const TransitionAction* action);

    /**
     * Set sub machine for a given state.
     *
     * @param stateId The id of the state to be set on
     * @param machine The sub state machine to be set
     * @return True if successful, false otherwise
     *
     * Note:
	 * If the stateId is FINAL_STATE_ID or invalid, it will
	 * return false;
	 * If a state with the given id does not exist, it will
	 * return false;
     * Return false when the state (specified by stateId) is loaded
     * into an engine which is at the moment already started.
     */
    bool setSubMachine(int stateId, StateMachine* machine);

    /**
     * Start the engine.
     *
     * @return error code
     */
    virtual error_code_t start();

    /**
     * Shut down the engine.
     *
     * @return error code
     */
    virtual error_code_t shutdown();

    /**
     * Post the given event to the engine so that to trigger a transition.
     *
     * @return error code
     */
    virtual error_code_t postEvent(int eventId);

  protected:

    /**
     * Process transition
     *
     * @param transition The transition to be processed
     * @return error code
     */
    virtual error_code_t processTransition(Transition& transition);

  private:
	static const int SUB_ENGINE_ID;

    StateMachineEngine(int id, StateMachineEngine* parent);
    StateMachineEngine();
    StateMachineEngine(int id, StateMachineEngine& engine);
    StateMachineEngine& operator = (const StateMachineEngine& engine);

    bool doSetStateAction(int stateId, const StateAction* action, bool entry);

    void _start();
    void _shutdown();
    error_code_t defaultEvent(int eventId);
    void _enterState(State* state);
    void _exitState(State* state);
    error_code_t _doTransition(Transition& transition);

    int id;
    StateMachine* machine;
    State* activeState;
    StateMachineEngine* parent;
//    int transitionStackDepth;
    bool transitionInProcess;
//    std::queue<Transition*> transitionQueue;

};

}}}//end of namespace

#endif /* STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1 */
