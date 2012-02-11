/***************************************************/
/*! \class ADSREnvOld
\brief ADSR envelope class.

This class implements a traditional ADSR (Attack, Decay, Sustain,
Release) envelope.  It responds to simple keyOn and keyOff
messages, keeping track of its state.  The \e state = ADSREnvOld::DONE
after the envelope value reaches 0.0 in the ADSREnvOld::RELEASE state.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "ADSREnvOld.h"


ADSREnvOld::ADSREnvOld( void )
{
	target_ = 0.0;
	value_ = 0.0;

	//attackRate_ = 0.001f;
	//decayRate_ = 0.001f;
	//releaseRate_ = 0.005f;
	//sustainLevel_ = 0.5f;
	setAllTimes(0.1f, 0.5f, 1.0f, 5.0f );

	state_ = STATE_DONE;
	addSampleRateAlert( this );
}


ADSREnvOld::~ADSREnvOld( void )
{
}


void ADSREnvOld::sampleRateChanged( FLOAT newRate, FLOAT oldRate )
{
	if( !ignoreSampleRateChange_ ) 
	{
		attackRate_ = oldRate * attackRate_ / newRate;
		decayRate_ = oldRate * decayRate_ / newRate;
		releaseRate_ = oldRate * releaseRate_ / newRate;
	}
}


void ADSREnvOld::setAttackRate( FLOAT rate )
{
	if( rate < 0.0 ) {
		negativeRatesError();
		attackRate_ = -rate;
	}
	else attackRate_ = rate;
}


void ADSREnvOld::setDecayRate( FLOAT rate )
{
	if( rate < 0.0 ) {
		negativeRatesError();
		decayRate_ = -rate;
	}
	else decayRate_ = rate;
}


void ADSREnvOld::setSustainLevel( FLOAT level )
{
	if( level < 0.0 ) {
		negativeRatesError();
		sustainLevel_ = 0.0;
	}
	else sustainLevel_ = level;
}


void ADSREnvOld::setReleaseRate( FLOAT rate )
{
	if( rate < 0.0 ) {
		negativeRatesError();
		releaseRate_ = -rate;
	}
	else releaseRate_ = rate;
}


void ADSREnvOld::setAttackTime( FLOAT time )
{
	if( time < 0.0 ) {
		negativeRatesError();
		time *= -1;
	}
	attackRate_ = 1.0f / ( time * sampleRate() );
}


void ADSREnvOld::setDecayTime( FLOAT time )
{
	if( time < 0.0 ) {
		negativeRatesError();
		time *= -1;
	}
	decayRate_ = 1.0f / ( time * sampleRate() );
}


void ADSREnvOld::setReleaseTime( FLOAT time )
{
	if( time < 0.0 ) {
		negativeRatesError();
		time *= -1;
	}
	releaseRate_ = sustainLevel_ / ( time * sampleRate() );
}


void ADSREnvOld::setAllTimes( FLOAT aTime, FLOAT dTime, FLOAT sLevel, FLOAT rTime )
{
	setAttackTime( aTime );
	setDecayTime( dTime );
	setSustainLevel( sLevel );
	setReleaseTime( rTime );
}


void ADSREnvOld::setParameter( UINT16 paramId, FLOAT value ) 
{
	switch( paramId ) {
	case PARAM_ATTACK:  setAttackTime( value ); break;
	case PARAM_DECAY:   setDecayTime( value ); break;
	case PARAM_SUSTAIN: setSustainLevel( value ); break;
	case PARAM_RELEASE: setReleaseTime( value ); break;
	}
}


FLOAT ADSREnvOld::getParameter( UINT16 paramId ) 
{
	FLOAT result = 0.0f;

	switch( paramId ) {
	case PARAM_ATTACK:  result = 1.0f / ( attackRate_ * sampleRate() );
	case PARAM_DECAY:   result = 1.0f / ( decayRate_ * sampleRate() );
	case PARAM_SUSTAIN: result = sustainLevel_;
	case PARAM_RELEASE: result = 1.0f / ( releaseRate_ * sampleRate() );
	}
	return result;
}


void ADSREnvOld::update( UINT16 inputId, FLOAT value )
{
	if( inputId == MIDI_GATE ) {
		if( value > 0 ) {
			keyOn();
		}
		else {
			keyOff();
		}
	}
}


void ADSREnvOld::updateEvent( UINT16 inputId, FLOAT value )
{
	if( inputId == MIDI_GATE ) {
		if( value > 0 ) {
			keyOn();
		}
		else {
			keyOff();
		}
	}
}


void ADSREnvOld::keyOn()
{
	target_ = 1.0;
	state_ = STATE_ATTACK;
}


void ADSREnvOld::keyOff()
{
	target_ = 0.0;
	state_ = STATE_RELEASE;
}


void ADSREnvOld::setTarget( FLOAT target )
{
	target_ = target;
	if( value_ < target_ ) {
		state_ = STATE_ATTACK;
		setSustainLevel( target_ );
	}
	if( value_ > target_ ) {
		setSustainLevel( target_ );
		state_ = STATE_DECAY;
	}
}


//void ADSREnvOld::setValue( FLOAT value )
//{
//	state_ = STATE_SUSTAIN;
//	target_ = value;
//	value_ = value;
//	setSustainLevel( value );
//	lastFrame_[0] = value;
//}


void ADSREnvOld::negativeRatesError()
{
	throw runtime_error( "ADSREnvOld::setAttackTime: negative times not allowed ... correcting!" );
}


//------------------------------------------------------------------
// ADSRProxy
//------------------------------------------------------------------

ADSRProxy::ADSRProxy() : PolyProxy()
{
	addParameter( ADSREnvOld::PARAM_ATTACK );
	addParameter( ADSREnvOld::PARAM_DECAY );
	addParameter( ADSREnvOld::PARAM_RELEASE );
	addParameter( ADSREnvOld::PARAM_SUSTAIN );
}

