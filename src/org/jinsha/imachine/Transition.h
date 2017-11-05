#ifndef TRANSITION_H_HEADER_INCLUDED_BA47E601
#define TRANSITION_H_HEADER_INCLUDED_BA47E601

#include <string>
#include <boost/signal.hpp>

#include "State.h"

//##ModelId=45B6F94A03AC
class Transition
{
  public:
    //##ModelId=45B81D8E03E4
    Transition(int id, std::string name, int fromStateId, int ToStateId);

    //##ModelId=46027E700150
    ~Transition();
    //##ModelId=460C1C570154
    int get_id() const;


    //##ModelId=460C1C570172
    std::string const& get_name() const;

      
    
    


    //##ModelId=45D2BAD40279
    static const int INVALID_ID;



  private:
    //##ModelId=45B6FA09035E
              int id;
    //##ModelId=45B6FA8C02AE
    std::string name;
    //##ModelId=45B6FAB802B3
    int fromStateId;
    //##ModelId=45B6FAC802FD
    int toStateId;

    //##ModelId=46027E5D013F
	boost::signal0<void>* signal;
    //##ModelId=460A811E03D4
    	friend class StateMachineEngine;
};



#endif /* TRANSITION_H_HEADER_INCLUDED_BA47E601 */
