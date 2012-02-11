
#include "Master.h"



Master::Master() : 
    Module(), volume_( 0.1f ),
    ptrAudioIn_( NULL )
{
	inPorts_.push_back( &audioIn_ );
    ptrProcessAudio_ = static_cast< PtrProcess >( &Master::processAudio );
}


void Master::initInPorts() 
{
    ASSERT( numVoices_ == 1 );
    ptrAudioIn_ = audioIn_.setNumVoices( numVoices_ );
}


void Master::setParameter( UINT16 paramId, FLOAT value, FLOAT modulation, INT16 voice )
{
    switch( paramId ) {
	case PARAM_VOLUME: volume_ = value; break;
	}
}


void Master::processAudio() throw()
{
    FLOAT input  = *ptrAudioIn_;
    *ptrAudioIn_ = 0.0f;
    value_       = max( -1, min( 1, input * volume_ ));
}
