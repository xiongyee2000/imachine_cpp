#include "State.h"
#include "Transition.h"
#include "StateMachine.h"
#include "StateMachineEngine.h"

//#define SME_DEBUG_TO_CONSOLE
//#define SME_DEBUG_TO_FILE

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
		transitionInProcess(false),
		logFileName(""),
		logFile(nullptr)
{
#ifdef SME_DEBUG_TO_FILE
	if (parent != nullptr) {
		logFileName = parent->logFileName;
		logFile = parent->logFile;
	} else {
		logFileName = "imachine-";
		logFileName += id;
		logFileName += ".log";
		logFile = fopen(logFileName.c_str(), "w");
	}
#endif
}

int StateMachineEngine::loadStateMachine(StateMachine& stateMachine)
{
	if (machine != nullptr) {
		return ERR_MACHINE_LOADED;
	}

	if (stateMachine.engine != nullptr) {
		return ERR_MACHINE_LOADED;
	}

	if (!StateMachine::validate(stateMachine)) {
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
		return ERR_MACHINE_NOT_LOADED;
	}

	if (isStarted()) {
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
	if (stateId == State::FINAL_STATE_ID) return false; //FINAL_STATE not allowed to have sub state machine
	if (this->machine == nullptr) return false;

	auto iter = this->machine->states.find(stateId);
	if (iter == this->machine->states.end()) {
		return false;
	}

	State* state = iter->second;
	if (state->subMachine == nullptr) {
		state->subMachine = machine;
	} else {
		StateMachineEngine* subEngine = state->subMachine->engine;
		if (subEngine == nullptr) {
			state->subMachine = machine;
		} else {
			if (subEngine->isStarted()) { //Not allowed when sub-engine is started
				return false;
			}
			subEngine->unloadStateMachine(); //unload sub machine first
			delete subEngine; //the sub engine is managed by its parent engine, so delete it first.
			state->subMachine = machine;
			subEngine = new StateMachineEngine(SUB_ENGINE_ID, this);
			subEngine->loadStateMachine(*machine);
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
	if (stateId <= State::INVALID_STATE_ID) return false; //invalid state not allowed to have entry action
	if (this->machine == nullptr) return false;

	auto iter = this->machine->states.find(stateId);
	if (iter != this->machine->states.end()) {
		if (entry) {
			iter->second->entryAction = action;
		} else {
			iter->second->exitAction = action;
		}
		return true;
	} else {
		return false;
	}
}

bool StateMachineEngine::setTransitionAction(int transitionId, const TransitionAction* action)
{
	if (transitionId <= Transition::INVALID_ID) return false; //invalid transition not allowed to have entry action
	if (this->machine == nullptr) return false;

	auto iter = this->machine->transitions.find(transitionId);
	if (iter != this->machine->transitions.end()) {
		iter->second->action = action;
		return true;
	} else {
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
	if (parent != nullptr && logFile != nullptr) {
		fclose(logFile);
	}
}

void StateMachineEngine::_start() 
{
	if (machine->initState == nullptr) {
		debug("[" + machine->name + "] ");
		debug("Starting engine failed for initState is null.\n");
		return;
	}
	if (machine->initState->id == State::FINAL_STATE_ID) {
		debug("[" + machine->name + "] ");
		debug("Starting engine failed for initState is FINAL.\n");
		return;
	}

	debug("[" + machine->name + "] ");
	debug("Starting engine...\n");
	_enterState(machine->initState);
	debug("[" + machine->name + "] ");
	debug("Engine started.\n");
}

void StateMachineEngine::_shutdown()
{
	std::map<int, StateMachineEngine*>::iterator iter;

	debug("[" + machine->name + "] ");
	debug("Terminating engine...\n");

	if (!isStarted()) {
		debug("[" + machine->name + "] ");
		debug("Exception: engine not started!\n");
		return;
	}

	//If the active state is a sub machine, terminate the corresponding sub engine
	if (activeState->subMachine != nullptr &&
			activeState->subMachine->engine != nullptr &&
			activeState->subMachine->engine->isStarted()) {
		debug("[" + machine->name + "] ");
		debug("Sub engine is alive, terminate it...\n");
		activeState->subMachine->engine->_shutdown();
	}

	activeState = nullptr;

	debug("[" + machine->name + "] ");
	debug("Engine terminated.\n");
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
		debug("[" + machine->name + "] ");
		debug("Exception: engine is not started yet, unable to process transition!\n");
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

		debug("[" + machine->name + "] ");
		debug("Processing transition, id=");
		debug(transition.id);
		debug(" name=" + transition.name);
		debug("...\n");

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
			debug("[" + machine->name + "] ");
			debug("Exception: invalid transition for active state!\n");
			return ERR_IMPROPER_TRANSITION; //invalid transition for active state
		}

	return ERR_SUCCESS;
}

void StateMachineEngine::_enterState(State* state)
{
	debug("[" + machine->name + "] ");
	debug("Entering state: id=");
	debug(state->id);
	debug(" name=" + state->name);
	debug("...\n");

	activeState = state;

	if (state->id == State::FINAL_STATE_ID) {
		//If it is the FINAL state, terminate the engine
		debug("[" + machine->name + "] ");
		debug("Entered, active state:" + state->name + "\n");

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
		debug("[" + machine->name + "] ");
		debug("Entered, active state:" + state->name + "\n");

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
					debug("[" + machine->name + "] ");
					debug("Starting sub state machine...\n");
					subEngine->_start();
				}
			}
		}
	}
}

void StateMachineEngine::_exitState(State* state)
{
	debug("[" + machine->name + "] ");
	debug("Exiting state: id=");
	debug(state->id);
	debug(" name=" + state->name);
	debug("...\n");

	//If it is a sub state machine and the sub engine is running, terminate the sub engine
	if (state->subMachine != nullptr &&
			state->subMachine->engine != nullptr &&
			state->subMachine->engine->isStarted()) {
		debug("[" + machine->name + "] ");
		debug("The sub engine is alive, terminate it...\n");
		state->subMachine->engine->_shutdown();
	}

	if (state->exitAction != nullptr) {
		(*static_cast<const StateAction*>(state->exitAction))(this, state);
	}

	debug("[" + machine->name + "] ");
	debug("Exited from active state:" + state->name + "\n");
}

StateMachineEngine::error_code_t StateMachineEngine::postEvent(int eventId)
{
	if (isStarted()) {
		return defaultEvent(eventId);
	} else {
		debug("[" + machine->name + "] ");
		debug("Exception: engine is not started yet, unable to process event!\n");
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
	debug("[" + machine->name + "] ");
	debug("Default event handler received event, id=");
	debug(eventId);
	debug("\n");

	if 	(transitionInProcess == true) {
		debug("[" + machine->name + "] ");
		debug("Exception: Previous transition in process, unable to process another one!\n");
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
			debug("[" + machine->name + "] ");
			debug("Exception: unacceptable event for active state, id=");
			debug(activeState->id);
			debug("!\n");
			return ERR_UNACCEPTABLE_EVENT; //event not acceptable by the active state
		}
	} else {
		debug("[" + machine->name + "] ");
		debug("Exception: no active state");
		debug("!\n");
		return ERR_UNACCEPTABLE_EVENT; //event not acceptable by the active state
	}

	return errorCode;
}

StateMachineEngine::error_code_t StateMachineEngine::start()
{
	if (isStarted()) {
		debug("[" + machine->name + "] ");
		debug("Exception: engine already started!\n");
		return ERR_ENGINE_ALIVE; //already started
	} else {
		if 	(transitionInProcess == true) {
			debug("[" + machine->name + "] ");
			debug("Exception: Previous transition in process, unable to process another one!\n");
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
				debug("[" + machine->name + "] ");
				debug("Exception:  Start is forbidden for the parent engine in improper state!\n");
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
				debug("[" + machine->name + "] ");
				debug("Exception: Terminate is forbidden for the parent engine in improper state!\n");
				return ERR_FORBIDDEN;
			}
		}
	} else {
		debug("[" + machine->name + "] ");
		debug("Exception: engine not started!\n");
		return ERR_ENGINE_TERMINATED; //already terminated
	}

	return ERR_SUCCESS;
}

void StateMachineEngine::debug(const std::string& value)
{
#ifdef SME_DEBUG_TO_CONSOLE
	printf("%s", value.c_str());
#endif
#ifdef SME_DEBUG_TO_FILE
	if (logFile != nullptr) {
		fprintf(logFile, "%s", value.c_str());
		fflush(logFile);
	}
#endif
}

void StateMachineEngine::debug(bool value)
{
	const char* str = (value ? "true":"false");
#ifdef SME_DEBUG_TO_CONSOLE
	printf("%s", str);
#endif
#ifdef SME_DEBUG_TO_FILE
	if (logFile != nullptr) {
		fprintf(logFile, "%s", str);
		fflush(logFile);
	}
#endif
}

void StateMachineEngine::debug(int value)
{
#ifdef SME_DEBUG_TO_CONSOLE
	printf("%d", value);
#endif
#ifdef SME_DEBUG_TO_FILE
	if (logFile != nullptr) {
		fprintf(logFile, "%d", value);
		fflush(logFile);
	}
#endif
}

void StateMachineEngine::debug(const char *value)
{
#ifdef SME_DEBUG_TO_CONSOLE
	printf("%s", value);
#endif
#ifdef SME_DEBUG_TO_FILE
	if (logFile != nullptr) {
		fprintf(logFile, "%s", value);
		fflush(logFile);
	}
#endif
}

}}}//end of namespace


