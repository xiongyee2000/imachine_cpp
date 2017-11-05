#ifndef LISTENER_H_HEADER_INCLUDED_BA47EDD8
#define LISTENER_H_HEADER_INCLUDED_BA47EDD8

#include <string>
class StateMachineEngine;

//##ModelId=45B824400196
class Listener
{
  public:
    //##ModelId=45B8252201A2
    virtual void doStateEntryAction(StateMachineEngine& engine, int stateId) = 0;

    //##ModelId=45B825350178
    virtual void doStateExitAction(StateMachineEngine& engine, int stateId) = 0;

    //##ModelId=45B8257D0225
    virtual void doTransitionAction(StateMachineEngine& engine, int transitionId) = 0;

    //##ModelId=45B8265301A8
    virtual void doStateMachineStartAction(StateMachineEngine& engine) = 0;

    //##ModelId=45B826840284
    virtual void doStateMachineTerminateAction(StateMachineEngine& engine) = 0;
    //##ModelId=45BC223E02E1
    virtual const std::string& get_name() const = 0;

  
protected:

  private:


};



#endif /* LISTENER_H_HEADER_INCLUDED_BA47EDD8 */
