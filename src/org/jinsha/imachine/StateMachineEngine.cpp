#include <stdio.h>
#include "State.h"
#include "Transition.h"
#include "StateMachine.h"
#include "StateMachineEngine.h"

#ifdef DEBUG_LOG
#define log(fmt,...) do {\
		fprintf(stdout,fmt,##__VA_ARGS__); \
}\
while(0)
#else
#define log(fmt,...)
#endif

namespace org {namespace jinsha{namespace imachine{

const int StateMachineEngine::SUB_ENGINE_ID = -1;

StateMachineEngine::StateMachineEngine(int id) :
		StateMachineEngine(id, nullptr)
{
}

StateMachineEngine::StateMachineEngine(int id, StateMachineEngine* parent) :
		id(id),
		machine(nullptr),
		activeState(nullptr),
		parent(parent),
//		transitionStackDepth(0),
		transitionInProcess(false)
{
}

int StateMachineEngine::loadStateMachine(StateMachine& stateMachine)
{
	if (machine != nullptr) {
		log("[ENGINE:%d] Failed to load machine for another machine has already been loaded.\n", id);
		return ERR_MACHINE_LOADED;
	}

	if (stateMachine.engine != nullptr) {
		log("[ENGINE:%d] Failed to load machine for it has already been loaded into another engine.\n", id);
		return ERR_MACHINE_LOADED;
	}

	if (!StateMachine::validate(stateMachine)) {
		log("[ENGINE:%d] Failed to load machine for it is invalid.\n", id);
		return ERR_INVALID_STATE_MACHINE;
	}

	this->machine = &stateMachine;
	stateMachine.engine = this;

	//load sub machines
	for (auto item : this->machine->states) {
		if (item.second->subMachine != nullptr &&
			item.second->subMachine->engine == nullptr) {
			StateMachineEngine* subEngine = new StateMachineEngine(SUB_ENGINE_ID, this);
			subEngine->loadStateMachine(*item.second->subMachine);
		}
	}

	return ERR_SUCCESS;
}

int StateMachineEngine::unloadStateMachine()
{
	if (machine == nullptr) {
		log("[ENGINE:%d] Failed to unload machine for no machine is loaded.\n", id);
		return ERR_MACHINE_NOT_LOADED;
	}

	if (isStarted()) {
		log("[ENGINE:%d] Failed to unload machine when engine is started.\n", id);
		return ERR_ENGINE_ALIVE;
	}

	for (auto item : machine->states) {
		item.second->entryAction = nullptr;
		item.second->exitAction = nullptr;
		if (item.second->subMachine != nullptr &&
			item.second->subMachine->engine != nullptr) {
			item.second->subMachine->engine->unloadStateMachine();
			delete item.second->subMachine->engine;
		}
	}

	for (auto item : machine->transitions) {
		item.second->action = nullptr;
	}

	machine->engine = nullptr;
	machine = nullptr;

	return ERR_SUCCESS;
}

bool StateMachineEngine::setSubMachine(int stateId, StateMachine* machine)
{
	if (stateId == State::FINAL_STATE_ID) {
		log("[ENGINE:%d] Unable to set sub-machine for the state is FINAL.\n", id);
		return false; //FINAL_STATE not allowed to have sub state machine
	}
	if (machine == nullptr) {
		log("[ENGINE:%d] Unable to set a null sub-machine.\n", id);
		return false; //FINAL_STATE not allowed to have sub state machine
	}
	if (this->machine == nullptr) {
		log("[ENGINE:%d] Unable to set sub-machine for no machine is loaded.\n", id);
		return false;
	}

	auto iter = this->machine->states.find(stateId);
	if (iter == this->machine->states.end()) {
		log("[ENGINE:%d] Unable to set sub-machine for the state is not found.\n", id);
		return false;
	}

	log("[ENGINE:%d] Starting to set sub-machine (id=%d) to state (id=%d)...\n", id, machine->id, stateId);

	State* state = iter->second;
	if (state->subMachine == nullptr) {
		state->subMachine = machine;
		log("[ENGINE:%d] Sub-machine (id=%d) is set successfully to state (id=%d).\n", id, machine->id, stateId);
	} else {
		StateMachineEngine* subEngine = state->subMachine->engine;
		if (subEngine == nullptr) {
			state->subMachine = machine;
			log("[ENGINE:%d] Sub-machine (id=%d) is set successfully to state (id=%d).\n", id, machine->id, stateId);
		} else {
			if (subEngine->isStarted()) { //Not allowed when sub-engine is started
				log("[ENGINE:%d] Unable to set sub-machine to state (id=%d) when it holds a sub-engine which is already started.\n", id, state->id);
				return false;
			}
			if (subEngine->machine != nullptr) {
				log("[ENGINE:%d] Unload previous sub-machine (id=%d) from state (id=%d) first...\n", id, subEngine->machine->id, stateId);
				subEngine->unloadStateMachine(); //unload sub machine first
				log("[ENGINE:%d] Previous sub-machine (id=%d) of state (id=%d) is unloaded.\n", id, subEngine->machine->id, stateId);
				delete subEngine; //the sub engine is managed by its parent engine, so delete it first.
				log("[ENGINE:%d] Previous sub-engine (id=%d) is deleted.\n", id, subEngine->id);
			}
			state->subMachine = machine;
			log("[ENGINE:%d] Sub-machine (id=%d) is set successfully to state (id=%d).\n", id, machine->id, stateId);
			log("[ENGINE:%d] Creating a new engine for sub-machine (id=%d)...\n", id, machine->id);
			subEngine = new StateMachineEngine(SUB_ENGINE_ID, this);
			log("[ENGINE:%d] Done\n", id);
			log("[ENGINE:%d] Loading sub-machine (id=%d)...\n", id, machine->id);
			subEngine->loadStateMachine(*machine);
			log("[ENGINE:%d] Sub-machine (id=%d) loaded.\n", id, machine->id);
		}
	}

	return true;
}


bool StateMachineEngine::setStateEntryAction(int stateId, const StateAction* action)
{
	return doSetStateAction(stateId, action, true);
}

bool StateMachineEngine::setStateExitAction(int stateId, const StateAction* action)
{
	return doSetStateAction(stateId, action, false);
}

bool StateMachineEngine::doSetStateAction(int stateId, const StateAction* action, bool entry)
{
	if (stateId <= State::INVALID_STATE_ID) {
		log("[ENGINE:%d] Unable to set state action for the state (id=%d) is invalid.\n", id, stateId);
		return false; //invalid state not allowed to have action
	}
	if (this->machine == nullptr) {
		log("[ENGINE:%d] Unable to set state action for no machine is loaded.\n", id);
		return false;
	}

	auto iter = this->machine->states.find(stateId);
	if (iter != this->machine->states.end()) {
		if (entry) {
			iter->second->entryAction = action;
			log("[ENGINE:%d] State entry action set to state (id=%d) successfully.\n", id, stateId);
		} else {
			iter->second->exitAction = action;
			log("[ENGINE:%d] State exit action set to state (id=%d) successfully.\n", id, stateId);
		}
		return true;
	} else {
		log("[ENGINE:%d] Unable to set state action for the state (id=%d) is not found.\n", id, stateId);
		return false;
	}
}

bool StateMachineEngine::setTransitionAction(int transitionId, const TransitionAction* action)
{
	if (transitionId <= Transition::INVALID_ID) {
		log("[ENGINE:%d] Unable to set transition action for the transition (id=%d) is invalid.\n", id, transitionId);
		return false; //invalid transition not allowed to have action
	}
	if (this->machine == nullptr) {
		log("[ENGINE:%d] Unable to set transition action for no machine is loaded.\n", id);
		return false;
	}

	auto iter = this->machine->transitions.find(transitionId);
	if (iter != this->machine->transitions.end()) {
		iter->second->action = action;
		log("[ENGINE:%d] Transition action set to transition (id=%d) successfully.\n", id, transitionId);
		return true;
	} else {
		log("[ENGINE:%d] Unable to set transition action for the transition (id=%d) is not found.\n", id, transitionId);
		return false;
	}
}

StateMachineEngine::~StateMachineEngine()
{
	if (isStarted()) {
		_shutdown();
	}
	if (machine != nullptr) {
		unloadStateMachine();
	}
}

void StateMachineEngine::_start() 
{
	if (machine->initState == nullptr) {
		log("[ENGINE:%d] Failed to start engine for the init-state is null.\n", id);
		return;
	}
	if (machine->initState->id == State::FINAL_STATE_ID) {
		log("[ENGINE:%d] Failed to start engine for the init-state is FINAL.\n", id);
		return;
	}

	log("[ENGINE:%d] Starting engine...\n", id);
	_enterState(machine->initState);
	log("[ENGINE:%d] Engine started.\n", id);
}

void StateMachineEngine::_shutdown()
{
	std::map<int, StateMachineEngine*>::iterator iter;

	log("[ENGINE:%d] Shutting down engine...\n", id);

	if (!isStarted()) {
		activeState = nullptr;
		log("[ENGINE:%d] Engine is shut down.\n", id);
		return;
	}

	//If the active state is a sub machine, terminate the corresponding sub engine
	if (activeState->subMachine != nullptr &&
			activeState->subMachine->engine != nullptr &&
			activeState->subMachine->engine->isStarted()) {
		log("[ENGINE:%d] Sub-engine is alive, shut it down...\n", id);
		activeState->subMachine->engine->_shutdown();
	}

	activeState = nullptr;

	log("[ENGINE:%d] Engine is shut down.\n", id);
}

StateMachineEngine::error_code_t StateMachineEngine::processTransition(Transition& transition)
{
#if 0
	///////////////////////////////////////////////////////////////////////////////////////
	// The entry function may post an event to the engine, which will result in 
	// a recursive invocation to this method. To avoid the 'stack' like behavior,
	// we queue the transitions that are incurred by the transition.
	///////////////////////////////////////////////////////////////////////////////////////
	transitionStackDepth = 0;
#endif

	bool success = true;
	error_code_t rtv = ERR_SUCCESS;

	if (!isStarted()) {
		log("[ENGINE:%d] Exception: Failed to process transition, for the engine is not started yet.\n", id);
		return ERR_ENGINE_TERMINATED;
	}

#if 0
	transitionStackDepth++;
	transitionQueue.push(&transition);

	if (transitionStackDepth > 1) {
		transitionStackDepth--;
		return ERR_SUCCESS;
	}

	while(!transitionQueue.empty()) {
		Transition* frontTransition = transitionQueue.front();
		transitionQueue.pop();
		rtv = _doTransition(*frontTransition);
		if (rtv != ERR_SUCCESS) {
			success = false;
		}
	}

	transitionStackDepth--;

	if (success) {
		return ERR_SUCCESS;
	} else {
		return ERR_RECURSIVE_TRANSITION;
	}
#endif

	return _doTransition(transition);
}

StateMachineEngine::error_code_t StateMachineEngine::_doTransition(Transition& transition)
{
		State* fromState = transition.fromState;
		State* toState = transition.toState;

		log("[ENGINE:%d] Transition (id=%d) triggered.\n", id, transition.id);

		//if the transition is valid
		if (fromState == activeState) {
			//If fromStateId does not equal to toStateId, exit the from-state
			if (fromState != toState) {
				//exit active state
				_exitState(activeState);
			}

			//do transition actions
			if (transition.action != nullptr) {

				(*static_cast<const TransitionAction*>(transition.action))(this, &transition);
			}

			//If fromStateId does not equal to toStateId, enter the to-state
			if (fromState != toState) {
				//enter next state
				_enterState(toState);
			}
		} else {
			log("[ENGINE:%d] Exception: invalid transition (id=%d) for the active state (id=%d).\n", id, transition.id, activeState->id);
			return ERR_IMPROPER_TRANSITION; //invalid transition for active state
		}

	return ERR_SUCCESS;
}

void StateMachineEngine::_enterState(State* state)
{
	log("[ENGINE:%d] Entering state (id=%d)...\n", id, state->id);

	activeState = state;

	if (state->id == State::FINAL_STATE_ID) {
		//If it is the FINAL state, terminate the engine
		log("[ENGINE:%d] Entered state (id=%d).\n", id, state->id);

		_shutdown();

		//If this engine has a parent, and the active state of the parent has an exit-transition,
		//then do the exit transtion on parent engine.
		if (parent != nullptr &&
			parent->activeState != nullptr &&
			parent->activeState->subMachine != nullptr &&
			parent->activeState->subMachine->engine == this &&
			parent->activeState->exitTransition != nullptr) {
			parent->_doTransition(*parent->activeState->exitTransition);
		}
	} else {
		if (state->entryAction != nullptr) {
			(*static_cast<const StateAction*>(state->entryAction))(this, state);
		}

		log("[ENGINE:%d] Entered state (id=%d).\n", id, state->id);

		std::map<int, Transition*>::iterator autoTransition = state->triggers.find(StateMachine::AUTO_TRANSITION_EVENT);
		if (autoTransition != state->triggers.end()) {
			//If there is an auto-transition, exit current state, and do transition.
			_exitState(state);
			_doTransition(*autoTransition->second);
		} else {
			//If it is a sub state machine, and it is set to auto-start sub-machine,
			//then start the sub state machine engine
			if (state->subMachine != nullptr && state->autoStartSubMachine == true) {
				StateMachineEngine* subEngine = state->subMachine->engine;
				if (subEngine == nullptr) {
					subEngine = new StateMachineEngine(SUB_ENGINE_ID, this);
					subEngine->loadStateMachine(*state->subMachine);
				}
				if (!subEngine->isStarted()) {
					log("[ENGINE:%d] Starting sub-engine (id=%d)...\n", id, subEngine->id);
					subEngine->_start();
				}
			}
		}
	}
}

void StateMachineEngine::_exitState(State* state)
{
	log("[ENGINE:%d] Exiting state (id=%d)...\n", id, state->id);

	//If it is a sub state machine and the sub engine is running, terminate the sub engine
	if (state->subMachine != nullptr &&
			state->subMachine->engine != nullptr &&
			state->subMachine->engine->isStarted()) {
		log("[ENGINE:%d] The sub-engine is alive, shut it down...\n", id);
		state->subMachine->engine->_shutdown();
	}

	if (state->exitAction != nullptr) {
		(*static_cast<const StateAction*>(state->exitAction))(this, state);
	}

	log("[ENGINE:%d] Exited from state (id=%d).\n", id, state->id);
}

StateMachineEngine::error_code_t StateMachineEngine::postEvent(int eventId)
{
	if (isStarted()) {
		return defaultEvent(eventId);
	} else {
		log("[ENGINE:%d] Exception: Failed to process event (id=%d) for engine is not started yet.\n", id, eventId);
		return ERR_ENGINE_TERMINATED; //not started, unable to process event
	}
}

StateMachineEngine* StateMachineEngine::getSubEngine(int stateId) const
{
	if (stateId <= State::INVALID_STATE_ID) return nullptr;
	if (machine == nullptr) return nullptr;

	auto iter = machine->states.find(stateId);
	if (iter != machine->states.end()) {
		if (iter->second->getSubMachine() != nullptr) {
			return iter->second->getSubMachine()->engine;
		}
	}
	return nullptr;
}

bool StateMachineEngine::isStarted()
{
	return (activeState != nullptr && activeState->id != State::FINAL_STATE_ID);
}

StateMachineEngine::error_code_t StateMachineEngine::defaultEvent(int eventId)
{
	error_code_t errorCode = ERR_UNKOWN;
	log("[ENGINE:%d] Received event (id=%d).\n", id, eventId);

	if 	(transitionInProcess == true) {
		log("[ENGINE:%d] Exception: Failed to process event (id=%d), for the previous transition in progress.\n", id, eventId);
		return ERR_TRANSITION_IN_PROCESS;
	}

	if (activeState != nullptr && activeState->id != State::FINAL_STATE_ID) {
		Transition* transition = nullptr;
		std::map<int, Transition*>::iterator iter = activeState->triggers.find(eventId);
		if (iter != activeState->triggers.end()) {
			transition = iter->second;
			//process default transition
			transitionInProcess = true;
			errorCode = processTransition(*transition);
			transitionInProcess = false;
		} else {
			log("[ENGINE:%d] Exception: unacceptable event (id=%d) for active state (id=%d). \n", id, eventId, activeState->id);
			return ERR_UNACCEPTABLE_EVENT; //event not acceptable by the active state
		}
	} else {
		log("[ENGINE:%d] Exception: active state is either FINAL or null.\n", id);
		return ERR_UNACCEPTABLE_EVENT; //event not acceptable by the active state
	}

	return errorCode;
}

StateMachineEngine::error_code_t StateMachineEngine::start()
{
	if (isStarted()) {
		log("[ENGINE:%d] Exception: engine is already started.\n", id);
		return ERR_ENGINE_ALIVE; //already started
	} else {
		if 	(transitionInProcess == true) {
			log("[ENGINE:%d] Exception: Failed to start engine, for the previous transition in progress.\n", id);
			return ERR_TRANSITION_IN_PROCESS;
		}

		if (parent == nullptr) {
			//root engine, user can start it
			transitionInProcess = true;
			_start();
			transitionInProcess = false;
		} else { //It is a sub engine
			if (parent->isStarted() &&
				parent->activeState != nullptr &&
				parent->activeState->subMachine == nullptr &&
				parent->activeState->subMachine->engine == this) {
				transitionInProcess = true;
				_start();
				transitionInProcess = false;
			} else {
				log("[ENGINE:%d] Exception: Start is internally forbidden.\n", id);
				return ERR_FORBIDDEN;
			}
		}
	}

	return ERR_SUCCESS;
}

StateMachineEngine::error_code_t StateMachineEngine::shutdown()
{
	if (isStarted()) {
		if (parent == nullptr) {
			//root engine, user can terminate it
			_shutdown();
		} else { //It is a sub engine
			if (parent->isStarted() &&
				parent->activeState != nullptr &&
				parent->activeState->subMachine == nullptr &&
				parent->activeState->subMachine->engine == this) {
				_shutdown();
			} else {
				log("[ENGINE:%d] Exception: shutdown is internally forbidden.\n", id);
				return ERR_FORBIDDEN;
			}
		}
	} else {
		log("[ENGINE:%d] Exception: engine is not started.\n", id);
		return ERR_ENGINE_TERMINATED; //already terminated
	}

	return ERR_SUCCESS;
}

}}}//end of namespace


