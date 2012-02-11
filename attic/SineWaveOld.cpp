/***************************************************/
/*! \class SineWaveOld
\brief Sinusoid oscillator class.

This class computes and saves a static sine "table" that can be
shared by multiple instances. It has an interface similar to the
WaveLoop class but inherits from the Generator class. Output
values are computed using linear interpolation.

The "table" length, set in SineWaveOld.h, is 2048 samples by default.

by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "SineWaveOld.h"
#include <cmath>


Buffer SineWaveOld::table_;
unsigned int SineWaveOld::tableSize_ = 2048;


SineWaveOld::SineWaveOld() : Generator(),
	  time_(0.0), rate_(1.0), phaseOffset_(0.0)
{
	if( table_.empty() ) 
	{
		table_.resize( tableSize_ + 1, 1 );
		FLOAT temp = (FLOAT)(1.0 / tableSize_);

		for( unsigned long i=0; i<=tableSize_; i++ ) {
			table_[i] = sin( TWO_PI * i * temp );
		}
	}
	setFrequency( 440.0f );
	Module::addSampleRateAlert( this );
}


SineWaveOld::~SineWaveOld()
{
	Module::removeSampleRateAlert( this );
}


void SineWaveOld::sampleRateChanged( FLOAT newRate, FLOAT oldRate )
{
	if( !ignoreSampleRateChange_ ) {
		setRate( oldRate * rate_ / newRate );
	}
}


void SineWaveOld::reset( void )
{
	time_ = 0.0;
	lastFrame_[0] = 0;
}


void SineWaveOld::setFrequency( FLOAT frequency )
{
	// This is a looping frequency.
	setRate( tableSize_ * frequency / sampleRate() );
}


void SineWaveOld::update( UINT16 inputId, FLOAT value )
{
	if( inputId == MIDI_PITCH ) {
		setFrequency( value );
	}
}


void SineWaveOld::updateEvent( UINT16 inputId, FLOAT value )
{
	if( inputId == MIDI_PITCH ) {
		setFrequency( value );
	}
}


void SineWaveOld::addTime( FLOAT time )
{
	// Add an absolute time in samples.
	time_ += time;
}


void SineWaveOld::addPhase( FLOAT phase )
{
	// Add a time in cycles (one cycle = tableSize_).
	time_ += tableSize_ * phase;
}


void SineWaveOld::addPhaseOffset( FLOAT phaseOffset )
{
	// Add a phase offset relative to any previous offset value.
	time_ += ( phaseOffset - phaseOffset_ ) * tableSize_;
	phaseOffset_ = phaseOffset;
}

