#ifndef SINEWAVEOLD_H
#define SINEWAVEOLD_H


#include "BaseDef.h"
#include "Buffer.h"
#include "Generator.h"
#include "ModuleProxy.h"


/***************************************************/
/*! \class SineWaveOld
\brief Sinusoid oscillator class.

This class computes and saves a static sine "table" that can be
shared by multiple instances. It has an interface similar to the
WaveLoop class but inherits from the Generator class. Output
values are computed using linear interpolation.

The "table" length, set in SineWaveOld.h, is 2048 samples by default.

by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

class SineWaveOld : public Generator
{
public:
	SineWaveOld();
	~SineWaveOld( void );

	//! Clear output and reset time pointer to zero.
	void reset( void );

	//! Set the data read rate in samples.  The rate can be negative.
	//!If the rate value is negative, the data is read in reverse order.
	void setRate( FLOAT rate ) { rate_ = rate; };

	//! Set the data interpolation rate based on a looping frequency.
	/*!
	This function determines the interpolation rate based on the file
	size and the current Stk::sampleRate.  The \e frequency value
	corresponds to file cycles per second.  The frequency can be
	negative, in which case the loop is read in reverse order.
	*/
	void setFrequency( FLOAT frequency );

	void update( UINT16 inputId, FLOAT value );

	//! Increment the read pointer by \e time in samples, modulo the table size.
	void addTime( FLOAT time );

	//! Increment the read pointer by a normalized \e phase value.
	/*!
	This function increments the read pointer by a normalized phase
	value, such that \e phase = 1.0 corresponds to a 360 degree phase
	shift.  Positive or negative values are possible.
	*/
	void addPhase( FLOAT phase );

	//! Add a normalized phase offset to the read pointer.
	/*!
	A \e phaseOffset = 1.0 corresponds to a 360 degree phase
	offset.  Positive or negative values are possible.
	*/
	void addPhaseOffset( FLOAT phaseOffset );

	//! Return the last computed output value.
	FLOAT lastOut( void ) const { return lastFrame_[0]; };

	//! Compute and return one output sample.
	void process( void );

	//Buffer& process( Buffer& frames, unsigned int channel = 0 );

	enum EventId {
		MIDI_PITCH
	};


	UINT16 getNumAudioIn() { return 0; }

protected:
	void sampleRateChanged( FLOAT newRate, FLOAT oldRate );

	// proxy stuff
	void updateEvent( UINT16 inputId, FLOAT value );

	static Buffer table_;
	FLOAT time_;
	FLOAT rate_;
	FLOAT phaseOffset_;
	unsigned int iIndex_;
	FLOAT alpha_;

	static unsigned int tableSize_; // = 2048;
};


//--------------------------------------------------------
// SineWaveOldProxy
//--------------------------------------------------------

class SineWaveProxy : public PolyProxy 
{
public:
	Module* createModule() { return new SineWaveOld(); };
};


//--------------------------------------------------------
// inlines
//--------------------------------------------------------

inline void SineWaveOld::process( void )
{
	FLOAT temp = 0.0;
	if( sounding__ )
	{
		// Check limits of time address ... if necessary, recalculate modulo tableSize_.
		while( time_ < 0.0 ) {
			time_ += tableSize_;
		}
		while( time_ >= tableSize_ ) {
			time_ -= tableSize_;
		}
		iIndex_ = (UINT16)time_;
		alpha_ = time_ - iIndex_;
		temp = table_[ iIndex_ ];
		temp += ( alpha_ * ( table_[ iIndex_ + 1 ] - temp ) );

		// Increment time, which can be negative.
		time_ += rate_;
	}
	outputs_[0] = temp;
	lastFrame_[0] = temp;
}

/*
inline Buffer& SineWaveOld::process( Buffer& buffer, unsigned int channel )
{
#if defined(_STK_DEBUG_)
	if ( channel >= buffer.channels() ) {
		Exception::process( "SineWaveOldOld::tick(): channel and buffer arguments are incompatible!", Exception::FUNCTION_ARGUMENT );
	}
#endif

	FLOAT *samples = &buffer[channel];
	FLOAT tmp = 0.0;

	unsigned int hop = buffer.channels();
	for ( unsigned int i=0; i<buffer.numFrames(); i++, samples += hop ) {

		// Check limits of time address ... if necessary, recalculate modulo tableSize_.
		while ( time_ < 0.0 ) {
			time_ += tableSize_;
		}
		while ( time_ >= tableSize_ ) {
			time_ -= tableSize_;
		}

		iIndex_ = (unsigned int) time_;
		alpha_ = time_ - iIndex_;
		tmp = table_[ iIndex_ ];
		tmp += ( alpha_ * ( table_[ iIndex_ + 1 ] - tmp ) );
		*samples = tmp;

		// Increment time, which can be negative.
		time_ += rate_;
	}

	lastFrame_[0] = tmp;
	return buffer;
}
*/

#endif // SINEWAVEOLD_H

