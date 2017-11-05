#ifndef EVENT_H_HEADER_INCLUDED_BA46A95B
#define EVENT_H_HEADER_INCLUDED_BA46A95B

#include <string>
//#include "State.h"
//#include "Transition.h"

//namespace org {namespace jinsha{namespace imachine{namespace engine {


//##ModelId=45B703C20011
class Event
{
  public:
    //##ModelId=45D3D5880298
    struct TRANSITION_MAP_T
    {
        //##ModelId=45D3D6210006
        int stateId;
        //##ModelId=45D3D63803CD
        int transitionId;
    };

    static const int INVALID_ID = -1;

    //##ModelId=45B82BC400E9
    Event(int id, const std::string& name, TRANSITION_MAP_T** transitions, int transitionNum);
    //##ModelId=45D3F8A7035B
    Event(int id, const std::string& name, int stateId = INVALID_ID,
			int transitionId = INVALID_ID);
    //##ModelId=460C1C7D0326
    const std::string& get_name() const;
    //##ModelId=4618EEF30322
    virtual ~Event();









  private:
    //##ModelId=45B82CEB03DA
    int get_transitionId(int fromStateId) const;

    //##ModelId=45B703CA019A
                                int id;
    //##ModelId=45B82B7F01B3
    TRANSITION_MAP_T* * transitions;
    //##ModelId=45BC4346016D
    const std::string& name;
    //##ModelId=45D2AA0603D5
    int transitionNum;
    //##ModelId=460C07F10065
    friend class StateMachineEngine;
};

//}}}//end of namespace

#endif /* EVENT_H_HEADER_INCLUDED_BA46A95B */
