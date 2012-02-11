
#ifndef CLOCK_H
#define CLOCK_H

#include "Module.h"


class Clock : Module
{
public:
	Clock();
	void init();

	void tick();
	void setSampleRate( FLOAT sampleRate );
	virtual void sampleRateChanged( FLOAT newRate, FLOAT oldRate );

protected:
	long counter_;
	FLOAT maxCounter_;
	FLOAT frequency_;
};


inline Clock::Clock() 
	: counter_( 0 ),
	  maxCounter_( 0 ),
	  frequency_( 440 )
{
}


inline void Clock::init() {
	addSampleRateAlert( this );
}


inline void Clock::sampleRateChanged( FLOAT newRate, FLOAT oldRate )
{
	maxCounter_ = newRate * 1000;
}



inline void Clock::tick()
{
	if( counter_ >= maxCounter_ ) {
		counter_ = 0;
	}
	counter_++;
}









#endif // CLOCK_H