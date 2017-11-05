#include "Transition.h"


//##ModelId=45D2BAD40279
const int Transition::INVALID_ID = -1;

//##ModelId=45B81D8E03E4
Transition::Transition(int id, std::string name, int fromStateId, int ToStateId) : 
		id(id),
		name(name),
		fromStateId(fromStateId),
		toStateId(ToStateId),
		signal(nullptr)
{
	signal = new boost::signal0<void>;
}


//##ModelId=46027E700150
Transition::~Transition()
{
	if (signal != nullptr) delete signal;
}


















//##ModelId=460C1C570154
int Transition::get_id() const
{
    return id;
}


//##ModelId=460C1C570172
std::string const& Transition::get_name() const
{
    return name;
}


