
#include "Delay.h"


Delay::Delay()
	: Module(),
	delayTime_( 0 ),
	feedback_( 0 ),
	gain_( 0 ),
    buffer_( NULL ),
	bufferSize_( (UINT32)sampleRate_ ),
    ptrAudioIn_( NULL )
{
    inPorts_.push_back( &audioIn_ );
	outPorts_.push_back( &audioOut_ );
    ptrAudioFunc_ = static_cast< PtrAudioFunc >( &Delay::processAudio );
}


void Delay::initData()
{
	buffer_ = bufDelay_.resize( numVoices_ * bufferSize_ );
	cursor_ = bufCursor_.resize( numVoices_ );
}


void Delay::initInPorts()
{
    ptrAudioIn_ = audioIn_.setNumVoices( numVoices_ );
}


void Delay::setParameter( UINT16 paramId, FLOAT value, FLOAT modulation, INT16 voice ) 
{
	switch( paramId ) {
	case PARAM_DELAYTIME: delayTime_ = (INT64)( value * (FLOAT)(bufferSize_ - 1)); break;
	case PARAM_FEEDBACK:  feedback_ = value; break;
	case PARAM_GAIN:	  gain_ = value; break;
	}
}


void Delay::resume()
{
	bufDelay_.set( 0 );
}


void Delay::processAudio() throw()
{
	FLOAT output, input;
    UINT32 index;
    UINT16 v;
    
    for( v=0; v<numVoices_; v++ )
	{
        input          = ptrAudioIn_[v];
        ptrAudioIn_[v] = 0.f;

		index          = v * bufferSize_ + cursor_[v];
		output         = buffer_[index];

		buffer_[index] = input + output * feedback_;

		if( ++cursor_[v] >= delayTime_ ) {
			cursor_[v] = 0;
		}
        audioOut_.putAudio( input + output * gain_, v );
	}
}



