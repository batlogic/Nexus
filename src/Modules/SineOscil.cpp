/***************************************************/
/*! \class SineOscil
\brief Sinusoid oscillator class.

This class computes and saves a static sine "table" that can be
shared by multiple instances. It has an interface similar to the
WaveLoop class but inherits from the Generator class. Output
values are computed using linear interpolation.

The "table" length, set in SineOscil.h, is 2048 samples by default.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "SineOscil.h"
#include "../Utils.h"
#include <cmath>


Buffer<FLOAT> SineOscil::bufTable_;
FLOAT* SineOscil::table_;
UINT16 SineOscil::tableSize_ = 2048;


SineOscil::SineOscil() 
	: Module(),
	  //phaseOffset_( 0.0f ),
	  tuning_( 1.0 ),
	  fineTuning_( 1.0 ),
      ptrFmInput_( NULL ),
      ptrAmInput_( NULL )
{
	makeWaveTable();

	outPorts_.push_back( &audioOut_ );
    inPorts_.push_back( &fmInput_ );
    inPorts_.push_back( &amInput_ );
    checkPorts();
}


SineOscil::~SineOscil()
{
    int i=0;
}


void SineOscil::initData()
{
	phaseIndex_ = bufPhaseIndex_.resize( numVoices_, 0.0f );
	amplitude_  = bufAmplitude_.resize( numVoices_, 1.0f );
	increment_  = bufIncrement_.resize( numVoices_, 20.43356f );	// 440 Hz
	freq_       = bufFreq_.resize( numVoices_, 440.0f );
}


void SineOscil::initInPorts()
{
    ptrFmInput_ = fmInput_.setNumVoices( numVoices_ );
    ptrAmInput_ = amInput_.setNumVoices( numVoices_ );
}


void SineOscil::checkPorts()
{
    bool fm = fmInput_.isConnected();
    bool am = amInput_.isConnected();

    if( fm == false && am == false ) {
        ptrAudioFunc_ = static_cast< PtrAudioFunc >( &SineOscil::processAudio );
    }
    else if( fm == true && am == false ) {
        ptrAudioFunc_ = static_cast< PtrAudioFunc >( &SineOscil::processAudioFm );
    }
    else if( fm == false && am == true ) {
        ptrAudioFunc_ = static_cast< PtrAudioFunc >( &SineOscil::processAudioAm );
    }
    else if( fm == true && am == true ) {
        ptrAudioFunc_ = static_cast< PtrAudioFunc >( &SineOscil::processAudioFmAm );
    }
}



void SineOscil::setSampleRate( INT32 newRate, INT32 oldRate )
{
	Module::setSampleRate( newRate, oldRate );

	for( UINT16 i=0; i<numVoices_; i++ ) {
		increment_[ i ] = oldRate * increment_[ i ] / newRate;
	}
}


void SineOscil::setParameter( UINT16 paramId, FLOAT value, FLOAT modulation, INT16 voice )
{
    voice = min( numVoices_-1, voice );

    __assume( paramId < 4 );
    switch( paramId ) 
    {
	case PARAM_FREQUENCY:  
        if( voice >= 0 ) 
        {
            FLOAT freq;
            if( modulation < 0 ) {
                double pitch = Utils::freq2pitch( value );
                pitch        = ( pitch - 60 ) * modulation + 60;
                freq         = (FLOAT)Utils::pitch2freq( pitch );
            }
            else {
                freq = ( value - 261.62557f ) * modulation + 261.62557f;    
            }
        	freq_[ voice ] = freq;
	        setIncrement( voice );
        }
        break;
	case PARAM_AMPLITUDE: 
        if( voice >= 0 )
            amplitude_[ voice ] = value * modulation; 
        break;

	case PARAM_TUNING:     setTuning( value ); break;
    case PARAM_FINETUNING: setFineTuning( value ); break;
	}
}


void SineOscil::setTuning( FLOAT paramValue )
{
    double pitch = (double)(INT32)paramValue;
    pitch        = min( 84, pitch );              // -48: C0 (0Hz), C4 (261.63Hz), 84: C11 (33.488Hz)
	tuning_      = pow( 2, pitch / 12 );

	for( UINT16 i=0; i<numVoices_; i++ ) {
        setIncrement( i );
	}
}


void SineOscil::setFineTuning( FLOAT paramValue )
{
    double pitch = max( -48, min( 84, paramValue ));  // -48: C0 (0Hz), C4 (261.63Hz), 84: C11 (33.488Hz)
    fineTuning_  = pow( 2, paramValue / 12 );

	for( UINT16 i=0; i<numVoices_; i++ ) {
		setIncrement( i );
	}
}


//void SineOscil::addPhase( FLOAT phase )
//{
//	for( UINT16 i=0; i<numVoices_; i++ ) {
//		phaseIndex_[i] += tableSize_ * phase;
//	}
//}
//
//
//void SineOscil::addPhaseOffset( FLOAT phaseOffset )
//{
//	// Add a phase offset relative to any previous offset value.
//	for( UINT16 i=0; i<numVoices_; i++ ) {
//		phaseIndex_[i] += ( phaseOffset - phaseOffset_ ) * tableSize_;
//	}
//	phaseOffset_ = phaseOffset;
//}


void SineOscil::makeWaveTable()
{
	if( bufTable_.empty() ) 
	{
		table_     = bufTable_.resize( tableSize_ + 1 );
		FLOAT temp = (FLOAT)(1.0 / tableSize_);

		for( UINT16 i=0; i<=tableSize_; i++ ) {
			table_[i] = sin( TWO_PI * i * temp );
		}
	}
	ASSERT( table_ );
}


void SineOscil::processAudio() throw()
{
	UINT32 maxVoices = min( numVoices_, polyphony_->numSounding_ );
    UINT32 index, v, i;
    FLOAT tick, frac, pos;
    
	for( i=0; i<maxVoices; i++ )
	{
		v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
		pos = phaseIndex_[v];
        
		while( pos < 0.0 ) pos += tableSize_;         // Check limits of table address
		while( pos >= tableSize_ ) pos -= tableSize_;

        index = (UINT32)pos;
		frac  = pos - index;
		tick  = table_[index];
		tick += amplitude_[v] * frac * ( table_[index + 1] - tick );

    	phaseIndex_[v] = pos + increment_[v];                    
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioFm()
{
	UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
	FLOAT tick, pos, frac;
	UINT32 index, v, i;

	for( i=0; i<maxVoices; i++ )
	{
		v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
		pos = phaseIndex_[v];
        
        pos += ptrFmInput_[v];							// FM
        ptrFmInput_[v] = 0.f;
		
		while( pos < 0.0 ) pos += tableSize_;         // Check limits of table address
		while( pos >= tableSize_ ) pos -= tableSize_;

        index = (UINT32)pos;
		frac  = pos - index;
		tick  = table_[index];
		tick += amplitude_[v] * frac * ( table_[index + 1] - tick );

    	phaseIndex_[v] = pos + increment_[v]; 
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioAm()
{
	UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
	FLOAT tick, pos, frac;
	UINT32 index, v, i;

	for( i=0; i<maxVoices; i++ )
	{
		v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
		pos = phaseIndex_[v];
        
		while( pos < 0.0 ) pos += tableSize_;         // Check limits of table address
		while( pos >= tableSize_ ) pos -= tableSize_;

        index = (UINT16)pos;
		frac  = pos - index;
		tick  = table_[index];
		tick += frac * ( table_[index + 1] - tick );
        tick *= amplitude_[v] + ptrAmInput_[v];
        ptrAmInput_[v] = 0.f;

    	phaseIndex_[v] = pos + increment_[v];                     // table position, which can be negative.
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioFmAm()
{
	UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );
	FLOAT tick, pos, frac;
	UINT32 index, v, i;

	for( i=0; i<maxVoices; i++ )
	{
		v   = mono_ ? 0 : polyphony_->soundingVoices_[i];
		pos = phaseIndex_[v];
        
        pos += ptrFmInput_[v];                         // FM
        ptrFmInput_[v] = 0.f;
		
		while( pos < 0.0 ) pos += tableSize_;         // Check limits of table address
		while( pos >= tableSize_ ) pos -= tableSize_;

        index = (UINT16)pos;
		frac  = pos - index;
		tick  = table_[index];
		tick += frac * ( table_[index + 1] - tick );
        tick *= amplitude_[v] + ptrAmInput_[v];
        ptrAmInput_[v] = 0.f;

    	phaseIndex_[v] = pos + increment_[v];          
        audioOut_.putAudio( tick, v );
	}
}







