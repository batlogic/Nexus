#ifndef ADSRENVOLD_H
#define ADSRENVOLD_H


#include "Generator.h"
#include "BaseDef.h"
#include "ModuleProxy.h"



/***************************************************/
/*! \class ADSREnvOld
\brief ADSREnvOld envelope class.

This class implements a traditional ADSR (Attack, Decay, Sustain,
Release) envelope.  It responds to simple keyOn and keyOff
messages, keeping track of its state.  The \e state = ADSREnvOld::DONE
after the envelope value reaches 0.0 in the ADSREnvOld::RELEASE state.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/


//----------------------------------------------------------------
// ADSREnvOld
//----------------------------------------------------------------

class ADSREnvOld : public Generator
{
public:

	//! ADSR envelope states.
	enum State {
		STATE_ATTACK,   /*!< Attack */
		STATE_DECAY,    /*!< Decay */
		STATE_SUSTAIN,  /*!< Sustain */
		STATE_RELEASE,  /*!< Release */
		STATE_DONE      /*!< End of release */
	};

	enum ParamId {
		PARAM_ATTACK,
		PARAM_DECAY,
		PARAM_SUSTAIN,
		PARAM_RELEASE
	};

	enum EventId {
		MIDI_GATE
	};
	
	ADSREnvOld( void );
	~ADSREnvOld( void );

	void setAttackRate( FLOAT rate );
	void setDecayRate( FLOAT rate );
	void setSustainLevel( FLOAT level );
	void setReleaseRate( FLOAT rate );

	void setAttackTime( FLOAT time );
	void setDecayTime( FLOAT time );
	void setReleaseTime( FLOAT time );
	void setAllTimes( FLOAT aTime, FLOAT dTime, FLOAT sLevel, FLOAT rTime );

	void setParameter( UINT16 paramId, FLOAT value );
	FLOAT getParameter( UINT16 paramId );

	void update( UINT16 inputId, FLOAT value );

	void setTarget( FLOAT target );
	int getState( void ) const { return state_; };
	//void setValue( FLOAT value );
	FLOAT lastOut( void ) const { return lastFrame_[0]; };

	void process( void );
	//Buffer& process( Buffer& frames, unsigned int channel = 0 );

protected:  
	void keyOn( void );
	void keyOff( void );

	// proxy stuff
	void updateEvent( UINT16 inputId, FLOAT value );

	void sampleRateChanged( FLOAT newRate, FLOAT oldRate );
	void negativeRatesError();

	int state_;
	FLOAT value_;
	FLOAT target_;
	FLOAT attackRate_;
	FLOAT decayRate_;
	FLOAT releaseRate_;
	FLOAT sustainLevel_;
};


//----------------------------------------------------------------
// ADSRProxy
//----------------------------------------------------------------

class ADSRProxy : public PolyProxy 
{
public:
	ADSRProxy();

	Module* createModule() { return new ADSREnvOld(); };
};



//----------------------------------------------------------------
// inlines
//----------------------------------------------------------------


inline void ADSREnvOld::process( void )
{
	if( sounding__ ) {
		switch ( state_ ) 
		{
		case STATE_ATTACK:
			value_ += attackRate_;
			if( value_ >= target_ ) {
				value_ = target_;
				target_ = sustainLevel_;
				state_ = STATE_DECAY;
			}
			lastFrame_[0] = value_;
			break;

		case STATE_DECAY:
			value_ -= decayRate_;
			if( value_ <= sustainLevel_ ) {
				value_ = sustainLevel_;
				state_ = STATE_SUSTAIN;
			}
			lastFrame_[0] = value_;
			break;

		case STATE_RELEASE:
			value_ -= releaseRate_;
			if( value_ <= 0.0 ) {
				value_ = (FLOAT)0.0;
				state_ = STATE_DONE;
			}
			lastFrame_[0] = value_;
		}

		FLOAT input = accumulateInput( 0 );
		outputs_[0] = value_ * input;
	}
}

/*
inline Buffer& ADSREnvOld :: process( Buffer& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
	if( channel >= frames.channels() ) {
		throw std::invalid_argument( "ADSREnvOld::process(): channel and Buffer arguments are incompatible!" );
	}
#endif

	FLOAT *samples = &frames[channel];
	unsigned int hop = frames.channels();

	for( unsigned int i=0; i<frames.numFrames(); i++, samples += hop ) {
		*samples = ADSREnvOld::process();
	}
	return frames;
}
*/


#endif // ADSRENVOLD_H
