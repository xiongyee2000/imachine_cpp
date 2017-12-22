#ifndef STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1
#define STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1

#include <stdio.h>
#include <map>
#include <list>
//#include <queue>
#include <functional>

namespace org {namespace jinsha{namespace imachine{

class StateMachineEngine
{
public:
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

    StateMachineEngine(int id);
    virtual ~StateMachineEngine();

    int getId() const {return id;};
    StateMachine& getStateMachine() const {return *machine;};
    State* getCurrentState() const {return activeState;};
    StateMachineEngine* getParent() const {return parent;};
    StateMachineEngine* getSubEngine(int stateId) const;

    bool isStarted();

    int loadStateMachine(StateMachine& stateMachine);
    int unloadStateMachine();
    bool setStateEntryAction(int stateId, const StateAction* action);
    bool setStateExitAction(int stateId, const StateAction* action);
    bool setTransitionAction(int transitionId, const TransitionAction* action);

    /**
     * Note:
     * Return false when this is the final state
     * Return false when the state (specified by stateId) is loaded
     * into an engine which is at the moment already started.
     */
    bool setSubMachine(int stateId, StateMachine* machine);

    virtual error_code_t start();
    virtual error_code_t shutdown();
    virtual error_code_t postEvent(int eventId);

  protected:
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
