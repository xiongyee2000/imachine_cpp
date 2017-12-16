#ifndef STATE_H_HEADER_INCLUDED_BA47843C
#define STATE_H_HEADER_INCLUDED_BA47843C

#include <string>
#include <map>
#include <functional>

namespace org {namespace jinsha{namespace imachine{
class Transition;
class StateMachine;
class StateMachineEngine;

class State
{
	friend class Transition;
    friend class StateMachine;
    friend class StateMachineEngine;

public:
    static const int FINAL_STATE_ID;
    static const int INVALID_STATE_ID;
    static const std::string FINAL_STATE_NAME;

	const StateMachine& getStateMachine() const {return machine;};
	const StateMachine* getSubMachine() const {return subMachine;};
    int getId() const {return id;};
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
