
#include "Envelopes.h"
#include "Synth.h"



ADSREnv::ADSREnv() : 
	Module(),
	attackRate_( NULL ),
	decayRate_( NULL ),
	sustainLevel_( NULL ),
	releaseRate_( NULL ),
	sentinel_( false ),
    ptrAudioIn_( NULL )
{
	inPorts_.push_back( &audioIn_ );
	outPorts_.push_back( &audioOut_ );

    ptrProcessAudio_ = static_cast< PtrProcess >( &ADSREnv::processAudio );
}


void ADSREnv::initData()
{
	value_        = bufValue_.resize( numVoices_, 0 );
	target_       = bufTarget_.resize( numVoices_, 0 );
	delta_        = bufDelta_.resize( numVoices_, 0 );
    state_        = bufState_.resize( numVoices_, 0 );
    velocity_     = bufVelocity_.resize( numVoices_, 0 );
    
    attackRate_   = bufAttackRate_.resize( numVoices_, 0 );
    decayRate_    = bufDecayRate_.resize( numVoices_, 0 );
    sustainLevel_ = bufSustainLevel_.resize( numVoices_, 0 );
    releaseRate_  = bufReleaseRate_.resize( numVoices_, 0 );
}


void ADSREnv::initInPorts()
{
    ptrAudioIn_ = audioIn_.setNumVoices( numVoices_ );
}


void ADSREnv::setSampleRate( INT32 newRate, INT32 oldRate )
{
	Module::setSampleRate( newRate, oldRate );

	for( UINT16 i=0; i<numVoices_; i++ ) {
        attackRate_[i]  = oldRate * attackRate_[i]  / newRate;
        decayRate_[i]   = oldRate * decayRate_[i]   / newRate;
        releaseRate_[i] = oldRate * releaseRate_[i] / newRate;
    }
}


void ADSREnv::setParameter( UINT16 paramId, FLOAT value, FLOAT modulation, INT16 voice )
{
    voice = min( numVoices_-1, voice );

    switch( paramId )
    {
    case PARAM_GATE:
        if( voice > -1 )
        {
		    FLOAT velo = value * modulation + ( 1 - modulation );
            value > 0.0f ? keyOn( velo, voice ) : keyOff( voice );
            break;
	    }
	case PARAM_ATTACK:  setAttackRate( value, voice ); break;
	case PARAM_DECAY:   setDecayRate( value, voice ); break;
	case PARAM_SUSTAIN: setSustainLevel( value, voice ); break;
	case PARAM_RELEASE: setReleaseRate( value, voice ); break;
    }
}


void ADSREnv::setAttackRate( FLOAT time, INT16 voice )
{
    FLOAT rate = calcRate( time*0.71f );

	if( voice > -1 ) {
        attackRate_[voice] = rate;
    }
    else for( UINT16 i=0; i<numVoices_; i++ ) {	
        attackRate_[i] = rate;

        if( target_[i] == 2 ) {
			delta_[i] = attackRate_[i];
		}
	}
}


void ADSREnv::setDecayRate( FLOAT time, INT16 voice )
{
    FLOAT rate = calcRate( time*0.089f );

	if( voice > -1 ) {
        decayRate_[voice] = rate;
    }
    else for( UINT16 i=0; i<numVoices_; i++ ) {	
        decayRate_[i] = rate;
	}
}


void ADSREnv::setSustainLevel( FLOAT level, INT16 voice )
{
	if( voice > -1 ) {
        sustainLevel_[voice] = level;
    }
    else for( UINT16 i=0; i<numVoices_; i++ ) {
		sustainLevel_[i] = level;
	}
}


void ADSREnv::setReleaseRate( FLOAT time, INT16 voice )
{
    FLOAT rate = calcRate( time*0.106f );

	if( voice > -1 ) {
        releaseRate_[voice] = rate;
    }
	else for( UINT16 i=0; i<numVoices_; i++ ) {	
        releaseRate_[i] = rate;

		if( target_[i] == 0 ) {
			delta_[i] = releaseRate_[i];
		}
	}
}


void ADSREnv::keyOn( FLOAT amplitude, UINT16 voice )
{
    value_[voice]    = 0.0f;
    target_[voice]   = 1.f;
    state_[voice]    = StateAttack;

	delta_[voice]    = attackRate_[voice];
    velocity_[voice] = amplitude;
}


void ADSREnv::keyOff( UINT16 voice )
{
    target_[voice] = 0.0f;
    state_[voice]  = StateRelease;
	delta_[voice]  = releaseRate_[voice];
}


FLOAT ADSREnv::calcRate( FLOAT time )
{
    time       = max( 0.0001f, time );
    FLOAT rate = 1.0f / ((FLOAT)sampleRate_ * time * 2.f);

	return rate;
}


void ADSREnv::processAudio() throw()
{
    UINT32 maxVoices = min( numVoices_, polyphony_->numSounding_ );
    
    for( UINT32 i=0; i<maxVoices; i++ )
	{
		UINT32 v       = polyphony_->soundingVoices_[i];
        FLOAT input    = ptrAudioIn_[v];
        ptrAudioIn_[v] = 0.f;

        value_[v] += delta_[v] * ( target_[v] - value_[v] );

        audioOut_.putAudio( input * value_[v] * velocity_[v], v );
	}
}


void ADSREnv::processControl() throw()
{
    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );

	for( UINT16 i=0; i<maxVoices; i++ )
	{
        UINT16 v = polyphony_->soundingVoices_[ i ];

        __assume( state_[v] <= 4 );
        switch( state_[v] ) 
	    {
	    case StateAttack:
		    if( value_[v] >= target_[v] ) 
            {
                delta_[v]  = decayRate_[v];
			    target_[v] = sustainLevel_[v];
			    state_[v]  = StateDecay;
		    }
		    break;
	    case StateDecay:
		    if( value_[v] <= sustainLevel_[v] ) 
            {
                delta_[v] = 0.f;
			    state_[v] = StateSustain;
		    }
		    break;
	    case StateRelease:
		    if( value_[v] <= 0.0001f ) 
            {
			    state_[v] = StateDone;

                if( sentinel_ ) {
                    polyphony_->endVoice( v );
                }
		    }
            break;
	    }
	}
}




//void ADSREnv::processControl() throw()
//{
//    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
//
//	for( UINT16 i=0; i<maxVoices; i++ )
//	{
//        UINT16 v = polyphony_->soundingVoices_[ i ];
//
//		//if( value_[v] + target_[v] > ATTACK_TARGET_PLUS_VALUE )
//		//{
//  //          delta_[v]  = decayRate_[v];
//		//	target_[v] = sustainLevel_[v];
//		//}
//
//		if( sentinel_ ) {
//            if( state_[v] == StateDone ) {
//                polyphony_->endVoice( v );
//            }
//			//if( value_[v] < 0.0001f && delta_[v] == releaseRate_[v] ) 
//   //         {
//   //             polyphony_->endVoice( v );
//			//}
//		}
//	}
//}




