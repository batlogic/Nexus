
#include "Delay.h"


Delay::Delay()
	: Module(),
	delay_( 0 ),
	feedback_( 0 ),
	gain_( 0 ),
    buffer_( NULL ),
	bufferSize_( (UINT32)sampleRate_ ),
    ptrAudioIn_( NULL )
{
    inPorts_.push_back( &audioIn_ );
	outPorts_.push_back( &audioOut_ );
    ptrProcessAudio_ = static_cast< PtrProcess >( &Delay::processAudio );
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
	case PARAM_DELAY:	 delay_ = (INT64)( value * (FLOAT)(bufferSize_ - 1)); break;
	case PARAM_FEEDBACK: feedback_ = value; break;
	case PARAM_GAIN:	 gain_ = value; break;
	}
}


void Delay::resume()
{
	bufDelay_.set( 0 );
}


void Delay::processAudio() throw()
{
	FLOAT value, input;
    UINT32 index;
    UINT16 v;
    
    for( v=0; v<numVoices_; v++ )
	{
        input          = ptrAudioIn_[v];
        ptrAudioIn_[v] = 0.f;

		index          = v * bufferSize_ + cursor_[v];
		value          = buffer_[index];

		buffer_[index] = input + value * feedback_;

		if( ++cursor_[v] >= delay_ ) {
			cursor_[v] = 0;
		}
        audioOut_.putAudio( input + value * gain_, v );
	}
}



