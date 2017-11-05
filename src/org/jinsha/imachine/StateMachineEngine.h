#ifndef STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1
#define STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1

#include <stdio.h>
#include <map>
#include <list>
#include <queue>

#include <boost/bind.hpp>

#include "Event.h"
#include "Listener.h"
#include "State.h"
#include "StateMachine.h"
#include "Transition.h"

//##ModelId=45B6FA6D028A
class StateMachineEngine
{
	public:
    //##ModelId=45C9442E0039
    enum error_code_t { 
		SME_SUCCESS = 1024, 
		ENGINE_ALIVE_ERROR, 
		ENGINE_TERMINATED_ERROR, 
		TRANSITION_IN_PROCESS_ERROR, 
		IMPROPER_TRANSITION_ERROR, 
		INVALID_TRANSITION_ERROR, 
		INVALID_EVENT_ERROR, 
		IMPROPER_START_ERROR, 
		IMPROPER_TERMINATE_ERROR, 
		UNMAPPED_EVENT_ERROR, 
		RECURSIVE_TRANSITION_ERROR, 
		UNKOWN_ERROR
	};

    //##ModelId=46027F9900DF
	typedef                                boost::signal0<void>::slot_function_type slot_t;
    //##ModelId=460282E20283
    typedef                                                   boost::signals::connection  connection_t;


//	##ModelId=45B827B20177
    StateMachineEngine(const StateMachine& stateMachine);

    //##ModelId=45B87EB90080
    virtual ~StateMachineEngine();
    //##ModelId=4602833A0077
//    StateMachStateMachineEngine_t* connectStateEntrySlot(int stateId, slot_t func);

	//##ModelId=45B889210187
    virtual error_code_t event(int eventId);

    //##ModelId=45B9ED4601B3
    int get_sm_id() const;


    //##ModelId=45B9ED52023C
    std::string const& get_sm_name() const;

    //##ModelId=45BA2CC0030C
    StateMachineEngine(int stateId);

    //##ModelId=45B84C8E0133
    const State* get_currentState() const;

    //##ModelId=45BB438C006D
    bool get_isStarted() const;

    //##ModelId=45BB522F0042
    error_code_t start();

    //##ModelId=45BB5246030D
    error_code_t terminate();

    //##ModelId=45BB607002C0
//    StateMachineEngStateMachineEnginest;

    //##ModelId=45D2BE4D012E
    static bool validate(const StateMachine& machine);

    StateMachineEngine* get_subEngine(int stateId);
    //##ModelId=45BB607002C0
    StateMachineEngine* get_parent() const
    {
        return parent;
    }




//    //##ModelId=460285070188
//    StateMachineEngineStateMachineEnginectStateExitSlot(int stateId, slot_t func);
//
//    //##ModelId=460285D402B0
//    StateMachineEngine::cStateMachineEngineransitionSlot(int transitionId, slot_t func);
//
//    //##ModelId=460286900382
//    StateMachineEngine::connStateMachineEnginetSlot(slot_t func);
//
//    //##ModelId=46028719018A
//    StateMachineEngine::connectStateMachineEnginetionSlot(slot_t func);
//
//    //##ModelId=46092C700067
//    void disconnectSlot(connection_t *connection);
    




  

  protected:
    //##ModelId=45B711BA02F8
    error_code_t processTransition(int transitionId);
    //##ModelId=45BC0B4E0229
    void debug(const std::string& value);
	//##ModelId=45BC0CAB0177
    void debug(bool value);
    //##ModelId=45BC0CC80395
    void debug(int value);
    //##ModelId=45BC1E56026B
    void debug(const char *value);





  private:

    //##ModelId=45B865960389
    void enterState(const State* state);

    //##ModelId=45B865A300E9
    void exitState(const State* state);

    //##ModelId=45BB48040393
    error_code_t defaultEvent(int eventId);

    //##ModelId=45B70E9200C4
    void _start();

    //##ModelId=45B7116501BE
    void _terminate();
    //##ModelId=45D5438C01A4
    error_code_t doTransition(int transitionId);

    //##ModelId=45B6FDA702CE
    std::map<int, State*> stateTable;

    //##ModelId=45B6FE1101B6
    std::map<int, Transition*> transitionTable;

    //##ModelId=45B702AC036C
    const State* currentState;

    //##ModelId=45B7047F015F
    std::map<int, Event*> eventTable;

    //##ModelId=45B9E94A017A
    //##ModelId=45B9E94A017A
    const State * initState;

    //##ModelId=45B9ED120000
    int sm_id;

    //##ModelId=45B9ED22012F
    std::string sm_name;


    //##ModelId=45BA2B390287
    StateMachineEngine* parent;
    std::queue<int> transitionQueue;

//    //##ModelId=46093153039B
//    boost::signal0<void>* start_signal;
//
//    //##ModelId=460931C80105
//    boost::signal0<void>* terminate_signal;

    //##ModelId=46092D1E00B8
    std::map<connection_t*, connection_t*> connections;

    //##ModelId=45D551FA0088
    int transitionStackDepth;

    //##ModelId=45BD90840381
    bool transitionInProcess;

    //##ModelId=45C0508E0138
    FILE* logFile;

    //##ModelId=460DC6840224
    static     const char* LOG_FILE_NAME;


};



#endif /* STATEMACHINEENGINE_H_HEADER_INCLUDED_BA4604F1 */
