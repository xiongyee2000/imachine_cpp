#ifndef STATEMACHINE_H_HEADER_INCLUDED_BA465AF4
#define STATEMACHINE_H_HEADER_INCLUDED_BA465AF4

#include "Event.h"
#include "State.h"
#include "Transition.h"


//##ModelId=45B9D115037F
class StateMachine {
  public:
    //##ModelId=45B9D2740022
    StateMachine(int id,
		const char* name, 
		State** states, 
		int stateNum, 
		Transition** transitions, 
		int transitionNum, 
		Event** events, 
		int eventNum, 
		int initState);

  private:
    //##ModelId=45B9D12A0126
    State** states;

    //##ModelId=45B9D14F0379
    Transition** transitions;

    //##ModelId=45B9D15700E6
    int stateNum;

    //##ModelId=45B9D15E0349
    int transitionNum;

    //##ModelId=45B9D167014D
    Event** events;

    //##ModelId=45B9D16F0290
    int eventNum;


    //##ModelId=45B9D1770313
    int initState;

    //##ModelId=45B9D40F0205
    int id;
    //##ModelId=4618F8F201A5
    const char* name;

    friend class StateMachineEngine;

};



#endif /* STATEMACHINE_H_HEADER_INCLUDED_BA465AF4 */
