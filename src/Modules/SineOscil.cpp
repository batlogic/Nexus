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
	  phaseOffset_( 0.0f ),
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
	time_      = bufTime_.resize( numVoices_, 0.0f );
	amplitude_ = bufAmplitude_.resize( numVoices_, 1.0f );
	rate_      = bufRate_.resize( numVoices_, 20.43356f );	// 440 Hz
	freq_      = bufFreq_.resize( numVoices_, 440.0f );
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
        ptrProcessAudio_ = static_cast< PtrProcess >( &SineOscil::processAudio );
    }
    else if( fm == true && am == false ) {
        ptrProcessAudio_ = static_cast< PtrProcess >( &SineOscil::processAudioFm );
    }
    else if( fm == false && am == true ) {
        ptrProcessAudio_ = static_cast< PtrProcess >( &SineOscil::processAudioAm );
    }
    else if( fm == true && am == true ) {
        ptrProcessAudio_ = static_cast< PtrProcess >( &SineOscil::processAudioFmAm );
    }
}



void SineOscil::setSampleRate( INT32 newRate, INT32 oldRate )
{
	Module::setSampleRate( newRate, oldRate );

	for( UINT16 i=0; i<numVoices_; i++ ) {
		rate_[ i ] = oldRate * rate_[ i ] / newRate;
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
	        setRate( voice );
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
        setRate( i );
	}
}


void SineOscil::setFineTuning( FLOAT paramValue )
{
    double pitch = max( -48, min( 84, paramValue ));  // -48: C0 (0Hz), C4 (261.63Hz), 84: C11 (33.488Hz)
    fineTuning_  = pow( 2, paramValue / 12 );

	for( UINT16 i=0; i<numVoices_; i++ ) {
		setRate( i );
	}
}


void SineOscil::addPhase( FLOAT phase )
{
	// Add a time in cycles (one cycle = tableSize_).
	for( UINT16 i=0; i<numVoices_; i++ ) {
		time_[i] += tableSize_ * phase;
	}
}


void SineOscil::addPhaseOffset( FLOAT phaseOffset )
{
	// Add a phase offset relative to any previous offset value.
	for( UINT16 i=0; i<numVoices_; i++ ) {
		time_[i] += ( phaseOffset - phaseOffset_ ) * tableSize_;
	}
	phaseOffset_ = phaseOffset;
}


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
    UINT32 index;
	UINT32 v;
    UINT32 i;
    FLOAT tick;
	FLOAT alpha;
    FLOAT time;
    
	for( i=0; i<maxVoices; i++ )
	{
		v    = mono_ ? 0 : polyphony_->soundingVoices_[i];
		time = time_[v];
        
		while( time < 0.0 ) time += tableSize_;         // Check limits of time address
		while( time >= tableSize_ ) time -= tableSize_;

        index = (UINT32)time;
		alpha = time - index;
		tick  = table_[index];
		tick += alpha * ( table_[index + 1] - tick );
        tick *= amplitude_[v];

    	time_[v] = time + rate_[v];                     // Increment time, which can be negative.
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioFm()
{
	FLOAT tick, time;
	UINT32 index;
	FLOAT alpha;
	UINT16 v, i;
    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );

	for( i=0; i<maxVoices; i++ )
	{
		v    = mono_ ? 0 : polyphony_->soundingVoices_[i];
		time = time_[v];
        
        time          += ptrFmInput_[v];                // FM
        ptrFmInput_[v] = 0.f;
		
		while( time < 0.0 ) time += tableSize_;         // Check limits of time address
		while( time >= tableSize_ ) time -= tableSize_;

        index = (UINT32)time;
		alpha = time - index;
		tick  = table_[index];
		tick += alpha * ( table_[index + 1] - tick );
        tick *= amplitude_[v];

    	time_[v] = time + rate_[v];                     // Increment time, which can be negative.
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioAm()
{
	FLOAT tick;
	UINT32 index;
	FLOAT alpha, time;
	UINT16 v, i;
    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );

	for( i=0; i<maxVoices; i++ )
	{
		v    = mono_ ? 0 : polyphony_->soundingVoices_[i];
		time = time_[v];
        
		while( time < 0.0 ) time += tableSize_;         // Check limits of time address
		while( time >= tableSize_ ) time -= tableSize_;

        index = (UINT16)time;
		alpha = time - index;
		tick  = table_[index];
		tick += alpha * ( table_[index + 1] - tick );
        tick *= amplitude_[v] + ptrAmInput_[v];
        ptrAmInput_[v] = 0.f;

    	time_[v] = time + rate_[v];                     // Increment time, which can be negative.
        audioOut_.putAudio( tick, v );
	}
}


void SineOscil::processAudioFmAm()
{
	FLOAT tick;
	UINT32 index;
	FLOAT alpha, time;
	UINT16 v, i;
    UINT16 maxVoices = min( numVoices_, polyphony_->numSounding_ );

	for( i=0; i<maxVoices; i++ )
	{
		v    = mono_ ? 0 : polyphony_->soundingVoices_[i];
		time = time_[v];
        
        time          += ptrFmInput_[v];                // FM
        ptrFmInput_[v] = 0.f;
		
		while( time < 0.0 ) time += tableSize_;         // Check limits of time address
		while( time >= tableSize_ ) time -= tableSize_;

        index = (UINT16)time;
		alpha = time - index;
		tick  = table_[index];
		tick += alpha * ( table_[index + 1] - tick );
        tick *= amplitude_[v] + ptrAmInput_[v];
        ptrAmInput_[v] = 0.f;

    	time_[v] = time + rate_[v];                     // Increment time, which can be negative.
        audioOut_.putAudio( tick, v );
	}
}







