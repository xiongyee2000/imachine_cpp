#include "State.h"

#include "StateMachineEngine.h"
#include "StateMachine.h"

using namespace std;

//##ModelId=45B8937703CC
const int State::TERMINAL_STATE_ID = -1;
const char* State::TERMINAL_STATE_NAME = "TERMINAL";


//##ModelId=45B71BFF002C
State::State(int id, const std::string& name, StateMachineEngine* subEngine) : 
		id(id),
		name(name),
//		entry_signal(NULL),
//		exit_signal(NULL),
		subEngine(subEngine)
{
//	entry_signal = new boost::signal0<void>;
//	exit_signal = new boost::signal0<void>;
}

//##ModelId=46027C8C00BE
State::~State()
{
//	if (entry_signal != NULL) delete entry_signal;
//	if (exit_signal != NULL) delete exit_signal;
	if (subEngine != nullptr) delete subEngine;
}


//##ModelId=45B81C350085
std::string const& State::get_name() const
{
    return name;
}


//##ModelId=45B81C800028
int State::get_id() const
{
    return id;
}


//##ModelId=4602909A0143
State& State::operator=(const State& right)
{
	id = right.id;
	name = right.name;
//	entry_signal = right.entry_signal;
//	exit_signal = right.exit_signal;
	subEngine = right.subEngine;
	return *this;
}

//##ModelId=460A7FDD0148
State::State() :
		id(0),
		name(""),
//		entry_signal(NULL),
//		exit_signal(NULL),
		subEngine(nullptr)
{
}


