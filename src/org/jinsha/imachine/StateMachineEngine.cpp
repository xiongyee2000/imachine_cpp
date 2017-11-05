#include <assert.h>

#include "boost/bind.hpp"
#include "StateMachineEngine.h"

//#define SME_DEBUG_TO_CONSOLE
//#define SME_DEBUG_TO_FILE

 const char* StateMachineEngine::LOG_FILE_NAME = "imachine.log";



//##ModelId=45B827B20177
StateMachineEngine::StateMachineEngine(const StateMachine& stateMachine)
{
	int i = 0;
	State* tmpState = nullptr;
	Transition* tmpTransition = nullptr;
	Event* tmpEvent = nullptr;

	assert(validate(stateMachine));

	parent = nullptr;
	transitionInProcess = false;
	transitionStackDepth = 0;
	logFile = fopen(LOG_FILE_NAME, "a");


	sm_id = stateMachine.id;
	sm_name = stateMachine.name;

	//intialize the state table
	for (i = 0; i < stateMachine.stateNum; i++) {
		tmpState = stateMachine.states[i];
		stateTable[tmpState->id] = tmpState;
		if (tmpState->subEngine != nullptr) {
			tmpState->subEngine->parent = this;
		}
	}
	currentState = stateTable[State::TERMINAL_STATE_ID];

	//intialize the transition table
	for (i = 0; i < stateMachine.transitionNum; i++) {
		tmpTransition = stateMachine.transitions[i];
		transitionTable[tmpTransition->id] = tmpTransition;
	}

	//intialize the event table
	for (i = 0; i < stateMachine.eventNum; i++) {
		tmpEvent = stateMachine.events[i];
		eventTable[tmpEvent->id] = tmpEvent;
	}

	initState = stateTable[stateMachine.initState];
}


//##ModelId=45B87EB90080
StateMachineEngine::~StateMachineEngine()
{
	std::map<int,State*>::iterator theStateIterator = stateTable.end();
	std::map<int,Transition*>::iterator theTransitionIterator = transitionTable.end();
	std::map<int,Event*>::iterator theEventIterator = eventTable.end();
	std::map<connection_t*, connection_t*>::iterator connectionIterator = connections.end();

	//release the connection list
	for( connectionIterator = connections.begin(); connectionIterator != connections.end(); connectionIterator++ ) {
		connectionIterator->second->disconnect();
		delete connectionIterator->second;
	}
	connections.clear();

	//release the event objects
	for( theEventIterator = eventTable.begin(); theEventIterator != eventTable.end(); theEventIterator++ ) {
		delete theEventIterator->second;
	}
	//release the event table
	eventTable.clear();

	//release the transition objects
	for( theTransitionIterator = transitionTable.begin(); theTransitionIterator != transitionTable.end(); theTransitionIterator++ ) {
		delete theTransitionIterator->second;
	}
	//release the transition table
	transitionTable.clear();

	//release the state objects
	for( theStateIterator = stateTable.begin(); theStateIterator != stateTable.end(); theStateIterator++ ) {
		delete theStateIterator->second;
	}
	//release the state table
	stateTable.clear();


	if (logFile != nullptr) {
		fclose(logFile);
	}
}


//##ModelId=45B70E9200C4
void StateMachineEngine::_start() 
{
	std::list<Listener*>::iterator theIterator;

	if (get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("Exception: engine already started!\n");
		return; //already started
	} 

	debug("[" + sm_name + "] ");
	debug("Starting engine...\n");
	transitionInProcess = true;

	debug("[" + sm_name + "] ");
	debug("Entering init state...\n");
	enterState(initState);
	debug("[" + sm_name + "] ");
	debug("Engine started.\n");
}


//##ModelId=45B7116501BE
void StateMachineEngine::_terminate()
{
	std::list<Listener*>::iterator theIterator;
	std::map<int, StateMachineEngine*>::iterator iter;

	debug("[" + sm_name + "] ");
	debug("Terminating engine...\n");

	if (!get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("Exception: engine not started!\n");
		return; //
	}


	//If the current state is a sub machine, terminate the corresponding sub engine
	if (currentState->subEngine != nullptr && currentState->subEngine->get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("Sub engine is alive, terminate it...\n");
		currentState->subEngine->_terminate();
	}

	currentState = stateTable[State::TERMINAL_STATE_ID];

	debug("[" + sm_name + "] ");
	debug("Engine terminated.\n");
}

//##ModelId=45B711BA02F8
StateMachineEngine::error_code_t StateMachineEngine::processTransition(int transitionId) 
{
	///////////////////////////////////////////////////////////////////////////////////////
	// The entry function may post an event to the engine, which will result in 
	// a recursive invocation to this method. To avoid the 'stack' like behavioral,
	// we queue the transitions that are incurred by the transition.
	///////////////////////////////////////////////////////////////////////////////////////
	transitionStackDepth = 0;
	int frontTransitionId = 0;
	bool success = true;
	error_code_t rtv = SME_SUCCESS;

	if (!get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("Exception: engine is not started yet, unable to process transition!\n");
		return ENGINE_TERMINATED_ERROR;
	}

	if (transitionInProcess) {
		debug("[" + sm_name + "] ");
		debug("Exception: Previous transition in process, unable to process another one!\n");
		return TRANSITION_IN_PROCESS_ERROR;
	}

	transitionStackDepth++;
	transitionQueue.push(transitionId);

	if (transitionStackDepth > 1) {
		transitionStackDepth--;
		return SME_SUCCESS;
	}


	while(!transitionQueue.empty()) {
		frontTransitionId = transitionQueue.front();
		transitionQueue.pop();
		rtv = doTransition(frontTransitionId);
		if (rtv != SME_SUCCESS) {
			success = false;
		}
	}

	transitionStackDepth--;

	if (success) {
		return SME_SUCCESS;
	} else {
		return RECURSIVE_TRANSITION_ERROR;
	}
}


//##ModelId=45D5438C01A4
StateMachineEngine::error_code_t StateMachineEngine::doTransition(int transitionId)
{
	std::map<int, Transition*>::iterator iter = transitionTable.find(transitionId);

	if (iter != transitionTable.end()) {	//if the transition exists
		int fromStateId = iter->second->fromStateId;
		int toStateId = iter->second->toStateId;

		debug("[" + sm_name + "] ");
		debug("Processing transition, id=");
		debug(transitionId);
		debug(" name=" + iter->second->name);
		debug("...\n");

		//if the transition is valid
		if (fromStateId == currentState->id) {
			std::list<Listener*>::iterator theIterator;

			//If fromStateId does not equal to toStateId, exit the from-state
			if (fromStateId != toStateId) {
				//exit current state
				exitState(currentState);
			}

			//do transition actions
			iter->second->signal->operator()();

			//If fromStateId does not equal to toStateId, enter the to-state
			if (fromStateId != toStateId) {
				//enter next state
				enterState(stateTable[toStateId]);
			}
		} else {
			debug("[" + sm_name + "] ");
			debug("Exception: invalid transition for current state!\n");
			return IMPROPER_TRANSITION_ERROR; //invalid transition for current state
		}
	} else {
		debug("[" + sm_name + "] ");
		debug("Error: invalid transition!\n");
		return INVALID_TRANSITION_ERROR; //transition does not exist
	}

	return SME_SUCCESS;
}



//##ModelId=45B84C8E0133
 const State* StateMachineEngine::get_currentState() const
{
    return currentState;
}


//##ModelId=45B865960389
void StateMachineEngine::enterState(const State* state)
{
	std::list<Listener*>::iterator theIterator;

	debug("[" + sm_name + "] ");
	debug("Entering state: id=");
	debug(state->id);
	debug(" name=" + state->name);
	debug("...\n");

	//set current state before actions so that the entry actions can post another event
	//immediately
	currentState = state;
	transitionInProcess = false;

	//If it is a sub state machine, start the sub state machine engine
	if (state->subEngine != nullptr) {
		debug("[" + sm_name + "] ");
		debug("Starting sub state machine...\n");
		if (currentState == state) {
			//We must check if the current state is the one we want to enter, 
			//because the entry function may trigger a transition that cause
			//the current state being changed.
			stateTable[state->id]->subEngine->_start();
		}
	}

	debug("[" + sm_name + "] ");
	debug("Entered, current state:" + state->name + "\n");

	////If it is the TERMINAL state, terminate the engine
	//if (state->id == SM_State::TERMINAL_STATE_ID) {
	//	_terminate();
	//	return;
	//}

}


//##ModelId=45B865A300E9
void StateMachineEngine::exitState(const State* state)
{
	std::list<Listener*>::iterator theIterator;

	debug("[" + sm_name + "] ");
	debug("Exiting state: id=");
	debug(state->id);
	debug(" name=" + state->name);
	debug("...\n");

	transitionInProcess = true;

	//If it is a sub state machine and the sub engine is running, terminate the sub engine
	if (state->subEngine != nullptr && state->subEngine->get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("The sub engine is alive, terminate it...\n");
		stateTable[state->id]->subEngine->_terminate();
	}

	debug("[" + sm_name + "] ");
	debug("Exited from current state:" + state->name + "\n");
}


//##ModelId=45B889210187
StateMachineEngine::error_code_t StateMachineEngine::event(int eventId)
{
	if (get_isStarted()) {
		return defaultEvent(eventId);
	} else {
		debug("[" + sm_name + "] ");
		debug("Exception: engine is not started yet, unable to process event!\n");
		return ENGINE_TERMINATED_ERROR; //not started, unable to process event
	}
}


//##ModelId=45B9ED4601B3
int StateMachineEngine::get_sm_id() const
{
    return sm_id;
}


//##ModelId=45B9ED52023C
std::string const& StateMachineEngine::get_sm_name() const
{
    return sm_name;
}



//##ModelId=45BA2CC0030C
StateMachineEngine* StateMachineEngine::get_subEngine(int stateId)
{
	State* state = stateTable[stateId];

	if (state && state->subEngine) {
		return state->subEngine;
	} else {
		return nullptr;
	}
}



//##ModelId=45BB438C006D
bool StateMachineEngine::get_isStarted() const
{
	return (currentState != nullptr && currentState->id != State::TERMINAL_STATE_ID);
}



//##ModelId=45BB48040393
StateMachineEngine::error_code_t StateMachineEngine::defaultEvent(int eventId)
{
	std::map<int, Event*>::iterator iter;
	Event* event = nullptr;

	debug("[" + sm_name + "] ");
	debug("Default event handler received event, id=");
	debug(eventId);
	debug("\n");

	iter = eventTable.find(eventId);
	if (iter != eventTable.end()) {
		int transitionId = Transition::INVALID_ID;
		event = iter->second;
		transitionId = event->get_transitionId(currentState->id);

		//process default transition
		if ( transitionId != Transition::INVALID_ID) {
			return processTransition(transitionId);
		} else {
			debug("[" + sm_name + "] ");
			debug("Exception: unacceptable event for current state, id=");
			debug(currentState->id);
			debug("!\n");
			return UNMAPPED_EVENT_ERROR; //event not mapped to a transition
		}
	} else {
		debug("[" + sm_name + "] ");
		debug("Exception: invalid event!\n");
		return INVALID_EVENT_ERROR; //event not found
	}

	return SME_SUCCESS;
}


//##ModelId=45BB522F0042
StateMachineEngine::error_code_t StateMachineEngine::start()
{
	if (get_isStarted()) {
		debug("[" + sm_name + "] ");
		debug("Exception: engine already started!\n");
		return ENGINE_ALIVE_ERROR; //already started
	} else {
		if (parent == nullptr) {
			//root engine, user can start it
			_start();
		} else { //It is a sub engine
			int parentCurrStateId = parent->get_currentState()->id;
			if (parent->get_subEngine(parentCurrStateId) == this) {
				//parent engine is in the state this engine belongs to, user can start it
				_start();
			} else {
				debug("[" + sm_name + "] ");
				debug("Exception: Parent engine in improper state, start forbidden!\n");
				return IMPROPER_START_ERROR; //user can not start it directly
			}
		}
	}

	return SME_SUCCESS;
}

//##ModelId=45BB5246030D
StateMachineEngine::error_code_t StateMachineEngine::terminate()
{
	if (get_isStarted()) {
		if (parent == nullptr) {
			//root engine, user can terminate it
			_terminate();
		} else { //It is a sub engine
			int parentCurrStateId = parent->get_currentState()->id;
			if (parent->get_subEngine(parentCurrStateId) == this) {
				//parent engine is in the state this engine belongs to, user can terminate it
				_terminate();
			} else {
				debug("[" + sm_name + "] ");
				debug("Exception:  Parent engine in improper state, termination forbidden!\n");
				return IMPROPER_TERMINATE_ERROR; //user can not terminate it directly
			}
		}
	} else {
		debug("[" + sm_name + "] ");
		debug("Exception: engine not started!\n");
		return ENGINE_TERMINATED_ERROR; //already terminated
	}

	return SME_SUCCESS;
}




//##ModelId=45D2BE4D012E
bool StateMachineEngine::validate(const StateMachine& machine)
{
	return true;
}


//##ModelId=45BC0B4E0229
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

//##ModelId=45BC0CAB0177
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

//##ModelId=45BC0CC80395
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


//##ModelId=45BC1E56026B
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




