#ifndef STATE_H_HEADER_INCLUDED_BA47843C
#define STATE_H_HEADER_INCLUDED_BA47843C

#include <string>

#include <boost/signal.hpp>

class StateMachine;
class StateMachineEngine;


////////////////////////
// Definition of the state
////////////////////////
//##ModelId=45B6F6C3020B
class State
{
  public:
	//##ModelId=45B71BFF002C
    State(int id, const std::string& name, StateMachineEngine* subEngine = nullptr);

    //##ModelId=460A7FDD0148
    State();

    //##ModelId=4602909A0143
    State& operator=(const State& right);

    //##ModelId=45B81C350085
    std::string const& get_name() const;
  
    //##ModelId=45B81C800028
    int get_id() const;

    //##ModelId=46027C8C00BE
    virtual ~State();




    //##ModelId=45B8937703CC
    static const int TERMINAL_STATE_ID;
    //##ModelId=460DC9E801E6
    static const char* TERMINAL_STATE_NAME;



  private:
	//##ModelId=45B6F6DC0166
    int id;

    //##ModelId=45B6F6E703B1
    std::string name;

//    //##ModelId=46027B3C0382
//	boost::signal0<void>* entry_signal;
//
//    //##ModelId=46029EE60394
//    boost::signal0<void>* exit_signal;

    //##ModelId=45B8775D01F5
    StateMachineEngine* subEngine;
    //##ModelId=460A79FD0308
    friend class StateMachineEngine;



};



#endif /* STATE_H_HEADER_INCLUDED_BA47843C */
