
#include "Sink.h"
#include "Modules/SystemModules.h"


Sink::Sink() : 
	ModuleList(), 
	output_( NULL ), 
	numModules_( 0 ),
	controlRateDiv_( SAMPLERATE / CONTROLRATE ),
	counter_( 0 ),
    dummyMaster_( 0 )
{}


void Sink::cleanup()
{
	ModuleList::clear();
	numModules_ = size();
    output_     = &dummyMaster_;
}


bool Sink::contains( Module* module )
{
    for( UINT16 i=0; i<size(); i++ ) {
        if( operator[](i) == module )
            return true;
    }
    return false;
}


void Sink::setSampleRate( INT32 sampleRate )
{
    controlRateDiv_ = sampleRate / CONTROLRATE;
}


void Sink::compile( Module* master )
{
	reverse( begin(), end() );

	numModules_ = size();
	output_     = master ? &(dynamic_cast< Master* >(master)->value_) : &dummyMaster_;
}



